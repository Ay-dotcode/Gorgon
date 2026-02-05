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

class Path {
public:
  Path() = default;

  Path(const Path &) = delete;
  Path(Path &&) = default;

  Path &operator=(const Path &) = delete;
  Path &operator=(Path &&) = default;

  Path Duplicate() const {
    Path copy;
    copy.Commands = Commands;
    copy.Contours = Contours;
    copy.ActiveContourIndex = ActiveContourIndex;
    copy.ExpectsMoveTo = ExpectsMoveTo;
    return copy;
  }

  void Clear() {
    Commands.clear();
    Contours.clear();
    ActiveContourIndex = -1;
    ExpectsMoveTo = false;
  }

  bool IsEmpty() const { return Commands.empty(); }

  auto GetCommandCount() const { return Commands.size(); }

  auto GetContourCount() const { return Contours.size(); }

  const std::vector<PathCommand> &GetCommands() const { return Commands; }

  const std::vector<PathContour> &GetContours() const { return Contours; }

  void ReserveCommands(std::size_t count) { Commands.reserve(count); }

  void ReserveContours(std::size_t count) { Contours.reserve(count); }

  void BeginContour(bool isNegative = false) {
    FinalizeOpenContour();

    PathContour contour;
    contour.FirstCommandIndex = Commands.size();
    contour.CommandCount = 0;
    contour.IsNegative = isNegative ? 1 : 0;

    Contours.push_back(contour);
    ActiveContourIndex = Contours.size() - 1;
    ExpectsMoveTo = true;
  }

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

  struct FlattenedContour {
    Geometry::PointList<Geometry::Pointf> Points;
    bool IsNegative = false;
    bool IsClosed = false;
  };

  std::vector<FlattenedContour> Flatten(Float tolerance = 0.72f,
                                        bool enforceWinding = true) const {
    ASSERT(tolerance > 0, "Tolerance cannot be 0 or less");

    std::vector<FlattenedContour> out;
    out.reserve(Contours.size());

    for (const auto &contour : Contours) {
      if (contour.CommandCount == 0)
        continue;

      const std::size_t beginIndex = (std::size_t)contour.FirstCommandIndex;
      const std::size_t endIndex =
          beginIndex + (std::size_t)contour.CommandCount;

      ASSERT(endIndex <= Commands.size(),
             "Contour command range is out of bounds");

      FlattenedContour flat;
      flat.IsNegative = contour.Negative();

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
          flat.Points.Clear();
          flat.Points.Push(current);
          break;
        }
        case PathVerb::LineTo: {
          ASSERT(haveCurrent, "LineTo requires MoveTo");
          current = cmd.To;
          if (flat.Points.IsEmpty() || flat.Points.Back() != current)
            flat.Points.Push(current);
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
              if (flat.Points.IsEmpty() || flat.Points.Back() != p)
                flat.Points.Push(p);
            }
          }
          current = cmd.Cubic.To;
          break;
        }
        case PathVerb::Close: {
          ASSERT(haveCurrent, "Close requires MoveTo");
          flat.IsClosed = true;
          current = start;
          i = endIndex;
          break;
        }
        }
      }

      if (flat.Points.GetSize() < 2) {
        continue;
      }

      if (flat.Points.GetSize() >= 2 &&
          flat.Points.Front() == flat.Points.Back()) {
        flat.Points.Pop();
      }

      if (enforceWinding && flat.Points.GetSize() >= 3) {
        auto area2 = SignedArea2(flat.Points);
        bool isCCW = area2 > 0;

        // Convention: positive geometry -> CCW, negative geometry -> CW.
        bool wantCCW = !flat.IsNegative;
        if (isCCW != wantCCW) {
          std::reverse(flat.Points.begin(), flat.Points.end());
        }
      }

      out.push_back(std::move(flat));
    }

    return out;
  }

  std::vector<Geometry::PointList<Geometry::Pointf>>
  FlattenPointLists(Float tolerance = 0.72f, bool enforceWinding = true) const {
    auto flattened = Flatten(tolerance, enforceWinding);
    std::vector<Geometry::PointList<Geometry::Pointf>> ret;
    ret.reserve(flattened.size());
    for (auto &c : flattened) {
      ret.push_back(std::move(c.Points));
    }
    return ret;
  }

private:
  std::vector<PathCommand> Commands;
  std::vector<PathContour> Contours;

  int ActiveContourIndex = -1;
  bool ExpectsMoveTo = false;

  void FinalizeOpenContour() {
    if (ActiveContourIndex < 0) {
      return;
    }

    auto &contour = Contours[(std::size_t)ActiveContourIndex];
    contour.CommandCount =
        (uint32_t)Commands.size() - contour.FirstCommandIndex;

    ActiveContourIndex = -1;
    ExpectsMoveTo = false;
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
