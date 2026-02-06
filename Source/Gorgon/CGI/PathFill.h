#pragma once

#include "Path.h"
#include "Polygon.h"

namespace Gorgon {
namespace CGI {

/**
 * Convenience overload: flattens a Path into contour point lists, then fills
 * it.
 *
 * Defaults to non-zero winding (W_ = 0) so PathContour::IsNegative is
 * meaningful when used with Path::Flatten(..., enforceWinding=true).
 */
template <int S_ = GORGON_DEFAULT_SUBDIVISIONS, int W_ = 0,
          class F_ = SolidFill<>>
void Polyfill(Containers::Image &target, const Path &path,
              Float tolerance = 0.72f, bool enforceWinding = true,
              F_ fill = SolidFill<>{Graphics::Color::Black}) {
  auto pointLists = path.FlattenPointLists(tolerance, enforceWinding);
  Polyfill<S_, W_, Geometry::Pointf, F_>(target, pointLists, fill);
}

template <int S_ = GORGON_DEFAULT_SUBDIVISIONS, int W_ = 0,
          class F_ = SolidFill<>>
void Polyfill(Graphics::Bitmap &target, const Path &path,
              Float tolerance = 0.72f, bool enforceWinding = true,
              F_ fill = SolidFill<>{Graphics::Color::Black}) {
  if (target.HasData()) {
    Polyfill<S_, W_>(target.GetData(), path, tolerance, enforceWinding, fill);
  }
}

} // namespace CGI
} // namespace Gorgon