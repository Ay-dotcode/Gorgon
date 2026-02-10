#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>

#include "../Geometry/Point.h"
#include "../Utils/Assert.h"
#include "Bezier.h"

namespace Gorgon {
namespace CGI {

enum class PathVerb : uint8_t { MoveTo, LineTo, CubicTo, Close };

struct PathCubic {
  Geometry::Pointf C1;
  Geometry::Pointf C2;
  Geometry::Pointf To;
};

struct PathCommand {
  PathVerb Verb = PathVerb::Close;

  union {
    Geometry::Pointf To;
    PathCubic Cubic;
  };

  PathCommand() : To(0, 0) {}

  static PathCommand MoveTo(Geometry::Pointf to) {
    PathCommand cmd;
    cmd.Verb = PathVerb::MoveTo;
    cmd.To = to;
    return cmd;
  }

  static PathCommand LineTo(Geometry::Pointf to) {
    PathCommand cmd;
    cmd.Verb = PathVerb::LineTo;
    cmd.To = to;
    return cmd;
  }

  static PathCommand CubicTo(Geometry::Pointf c1, Geometry::Pointf c2,
                             Geometry::Pointf to) {
    PathCommand cmd;
    cmd.Verb = PathVerb::CubicTo;
    cmd.Cubic = {c1, c2, to};
    return cmd;
  }

  static PathCommand Close() {
    PathCommand cmd;
    cmd.Verb = PathVerb::Close;
    return cmd;
  }
};

struct PathContour {
  uint32_t FirstCommandIndex = 0;
  uint32_t CommandCount = 0;
  uint8_t IsNegative = 0;

  bool Negative() const { return IsNegative != 0; }
};

// Usage: Path p; p.MoveTo({0, 0}); p.LineTo({10, 0}); p.Close();
// auto pts = p.FlattenPointLists();
class Path {
public:
  Path(const Path &) = delete;
  Path &operator=(const Path &) = delete;

  void MoveTo(Geometry::Pointf to, bool isNegative = false) {
    if (ActiveContourIndex < 0 || !ExpectsMoveTo)
      BeginContour(isNegative);
    else
      Contours[ActiveContourIndex].IsNegative = isNegative ? 1 : 0;

    PushCommand(PathCommand::MoveTo(to));
    ExpectsMoveTo = false;
  }

  void LineTo(Geometry::Pointf to) {
    ASSERT(ActiveContourIndex >= 0 && !ExpectsMoveTo,
           "LineTo requires an active contour started by MoveTo");
    PushCommand(PathCommand::LineTo(to));
  }

  void CubicTo(Geometry::Pointf c1, Geometry::Pointf c2, Geometry::Pointf to) {
    ASSERT(ActiveContourIndex >= 0 && !ExpectsMoveTo,
           "CubicTo requires an active contour started by MoveTo");
    PushCommand(PathCommand::CubicTo(c1, c2, to));
  }

  void Close() {
    ASSERT(ActiveContourIndex >= 0 && !ExpectsMoveTo,
           "Close requires an active contour started by MoveTo");
    PushCommand(PathCommand::Close());
  }

  std::vector<Geometry::PointList<Geometry::Pointf>>
  FlattenPointLists(Float tolerance = 0.72f, bool enforceWinding = true) const {
    ASSERT(tolerance > 0, "Tolerance cannot be 0 or less");

    std::vector<Geometry::PointList<Geometry::Pointf>> out;
    out.reserve(Contours.size());

    for (const auto &contour : Contours) {
      if (contour.CommandCount == 0)
        continue;

      const std::size_t beginIndex = contour.FirstCommandIndex;
      const std::size_t endIndex = beginIndex + contour.CommandCount;

      ASSERT(endIndex <= Commands.size(),
             "Contour command range is out of bounds");

      Geometry::PointList<Geometry::Pointf> points;

      bool haveCurrent = false;
      Geometry::Pointf current(0, 0);
      Geometry::Pointf start(0, 0);

      for (std::size_t i = beginIndex; i < endIndex; i++) {
        const auto &cmd = Commands[i];

        if (!haveCurrent)
          ASSERT(cmd.Verb == PathVerb::MoveTo,
                 "Contours must start with MoveTo");

        switch (cmd.Verb) {
        case PathVerb::MoveTo: {
          current = cmd.To;
          start = current;
          haveCurrent = true;
          points.Clear();
          points.Push(current);
          break;
        }
        case PathVerb::LineTo: {
          ASSERT(haveCurrent, "LineTo requires MoveTo");
          current = cmd.To;
          if (points.IsEmpty() || points.Back() != current)
            points.Push(current);
          break;
        }
        case PathVerb::CubicTo: {
          ASSERT(haveCurrent, "CubicTo requires MoveTo");
          basic_Bezier<Geometry::Pointf> bez(current, cmd.Cubic.C1,
                                             cmd.Cubic.C2, cmd.Cubic.To);
          auto seg = bez.Flatten(tolerance);
          auto sz = seg.GetSize();
          if (sz > 0) {
            // Append, skipping the first point (it equals current).
            for (std::size_t pi = 1; pi < sz; pi++) {
              const auto &p = seg[pi];
              if (points.IsEmpty() || points.Back() != p)
                points.Push(p);
            }
          }
          current = cmd.Cubic.To;
          break;
        }
        case PathVerb::Close: {
          ASSERT(haveCurrent, "Close requires MoveTo");
          current = start;
          i = endIndex;
          break;
        }
        }
      }

      if (points.GetSize() < 2)
        continue;

      if (points.GetSize() >= 2 && points.Front() == points.Back())
        points.Pop();

      if (enforceWinding && points.GetSize() >= 3) {
        auto area2 = SignedArea2(points);
        bool isCCW = area2 > 0;

        // Convention: positive geometry -> CCW, negative geometry -> CW.
        bool wantCCW = !contour.Negative();
        if (isCCW != wantCCW)
          std::reverse(points.begin(), points.end());
      }
      out.push_back(std::move(points));
    }
    return out;
  }

private:
  std::vector<PathCommand> Commands;
  std::vector<PathContour> Contours;

  int ActiveContourIndex = -1;
  bool ExpectsMoveTo = false;

  void BeginContour(bool isNegative) {
    PathContour contour;
    contour.FirstCommandIndex = Commands.size();
    contour.CommandCount = 0;
    contour.IsNegative = isNegative ? 1 : 0;

    Contours.push_back(contour);
    ActiveContourIndex = Contours.size() - 1;
    ExpectsMoveTo = true;
  }

  void PushCommand(const PathCommand &cmd) {
    ASSERT(ActiveContourIndex >= 0,
           "Cannot add path commands without an active contour");
    Commands.push_back(cmd);

    auto &contour = Contours[(std::size_t)ActiveContourIndex];
    contour.CommandCount++;
  }

  static Float
  SignedArea2(const Geometry::PointList<Geometry::Pointf> &points) {
    // Returns twice the signed area. Positive means CCW.
    Float a = 0;
    auto n = points.GetSize();
    for (std::size_t i = 0; i < n; i++) {
      const auto &p0 = points[i];
      const auto &p1 = points[(i + 1) % n];
      a += p0.X * p1.Y - p1.X * p0.Y;
    }
    return a;
  }
};

} // namespace CGI
} // namespace Gorgon
