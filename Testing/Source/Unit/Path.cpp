#define CATCH_CONFIG_MAIN

#include <catch.h>

#include <Gorgon/CGI/Path.h>

using namespace Gorgon;
using namespace Gorgon::CGI;

namespace {

Float SignedArea2(const Geometry::PointList<Geometry::Pointf> &pts) {
  Float a = 0;
  auto n = pts.GetSize();
  for (std::size_t i = 0; i < n; i++) {
    const auto &p0 = pts[i];
    const auto &p1 = pts[(i + 1) % n];
    a += p0.X * p1.Y - p1.X * p0.Y;
  }
  return a;
}

} // namespace

TEST_CASE("Path flattens lines", "[Path]") {
  Path p;
  p.MoveTo({0, 0});
  p.LineTo({1, 0});
  p.LineTo({1, 1});

  auto flat = p.FlattenContours();
  REQUIRE(flat.size() == 1);
  REQUIRE(flat[0].Points.GetSize() == 3); // start + two lines
  REQUIRE_FALSE(flat[0].IsClosed);
}

TEST_CASE("Path Close enforces new contour and marks closed", "[Path]") {
  Path p;
  p.MoveTo({0, 0});
  p.LineTo({10, 0});
  p.LineTo({10, 10});
  p.Close();

  auto flat = p.FlattenContours();
  REQUIRE(flat.size() == 1);
  REQUIRE(flat[0].IsClosed);
  REQUIRE(flat[0].Points.GetSize() == 3);

  // After close, next drawing op must start with MoveTo; starting a new contour succeeds.
  p.MoveTo({20, 20});
  p.LineTo({25, 25});
  auto flat2 = p.FlattenContours();
  REQUIRE(flat2.size() == 2);
  REQUIRE_FALSE(flat2[1].IsClosed);
  REQUIRE(flat2[1].Points.Front() == Geometry::Pointf(20, 20));
  REQUIRE(flat2[1].Points.Back() == Geometry::Pointf(25, 25));
}

TEST_CASE("Path winding flips for negative contours", "[Path]") {
  Path positive;
  positive.MoveTo({0, 0});
  positive.LineTo({2, 0});
  positive.LineTo({2, 2});
  positive.LineTo({0, 2});
  positive.Close();

  auto posFlat = positive.FlattenContours();
  REQUIRE(posFlat.size() == 1);
  REQUIRE(SignedArea2(posFlat[0].Points) > 0); // CCW for positive

  Path negative;
  negative.MoveTo({0, 0}, true);
  negative.LineTo({2, 0});
  negative.LineTo({2, 2});
  negative.LineTo({0, 2});
  negative.Close();

  auto negFlat = negative.FlattenContours();
  REQUIRE(negFlat.size() == 1);
  REQUIRE(negFlat[0].IsNegative);
  REQUIRE(SignedArea2(negFlat[0].Points) < 0); // CW enforced
}

TEST_CASE("Path cubic flattens more with tighter tolerance", "[Path]") {
  Path p;
  p.MoveTo({0, 0});
  p.CubicTo({10, 20}, {20, -20}, {30, 0});

  auto coarse = p.FlattenContours(1.5f);
  auto fine = p.FlattenContours(0.1f);

  REQUIRE(fine.size() == 1);
  REQUIRE(coarse.size() == 1);
  REQUIRE(fine[0].Points.GetSize() >= coarse[0].Points.GetSize());
  REQUIRE(fine[0].Points.Front() == Geometry::Pointf(0, 0));
  REQUIRE(fine[0].Points.Back() == Geometry::Pointf(30, 0));
}

TEST_CASE("Path TransformPoints applies to commands", "[Path]") {
  Path p;
  p.MoveTo({1, 1});
  p.LineTo({2, 2});
  p.CubicTo({3, 3}, {4, 4}, {5, 5});

  p.TransformPoints([](Geometry::Pointf &pt) {
    pt.X += 10;
    pt.Y += 1;
  });

  auto flat = p.FlattenContours();
  REQUIRE(flat.size() == 1);
  REQUIRE(flat[0].Points.Front() == Geometry::Pointf(11, 2));
  REQUIRE(flat[0].Points.Back() == Geometry::Pointf(15, 6));
}