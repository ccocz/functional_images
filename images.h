#ifndef BMP_IMAGE_H
#define BMP_IMAGE_H

#include <functional>
#include <cmath>
#include <utility>
#include "functional.h"
#include "coordinate.h"
#include "color.h"

template<typename T>
using Base_image = std::function<T(const Point)>;

using Fraction = double;

using Region = Base_image<bool>;

using Image = Base_image<Color>;

using Blend = Base_image<Fraction>;

template<typename T>
Base_image<T> constant(T t) {
  return [=]([[maybe_unused]] const Point p) {
    return t;
  };
}

template<typename T>
Base_image<T> rotate(Base_image<T> image, double phi) {
  return compose(
    [=](const Point p) {
      return from_polar(Point(to_polar(p).first, to_polar(p).second - phi, true));
    }, image);
}

template<typename T>
Base_image<T> translate(Base_image<T> image, Vector v) {
  auto change = [v](const Point p) {
    return Point(p.first - v.first, p.second - v.second, false);
  };
  return compose(change, image);
}

template<typename T>
Base_image<T> scale(Base_image<T> image, double s) {
  auto change = [s](Point p) {
    return Point(p.first / s, p.second / s);
  };
  return compose(change, image);
}

template<typename T>
Base_image<T> circle(Point q, double r, T inner, T outer) {
  return [=](const Point p) {
    return distance(p, q) <= r ? inner : outer;
  };
}

template<typename T>
Base_image<T> checker(double d, T this_way, T that_way) {
  return [=](const Point p) {
    return (static_cast<int>(std::floor(p.first / d) + std::floor(p.second / d)) % 2 == 0) ? this_way : that_way;
  };
}

template<typename T>
Base_image<T> polar_checker(double d, int n, T this_way, T that_way) {
  return compose([=](const Point p) {
    Point point = to_polar(p);
    return (Point(point.first, point.second * n * d / (2 * M_PI), true));
  }, checker(d, this_way, that_way));
}

template<typename T>
Base_image<T> rings(Point q, double d, T this_way, T that_way) {
  return [=](const Point p) {
    return (static_cast<int>(std::floor(distance(p, q) / d)) % 2 == 0) ? this_way : that_way;
  };
}

template<typename T>
Base_image<T> vertical_stripe(double d, T this_way, T that_way) {
  return [=](const Point p) {
    return (std::abs(p.first) <= d / 2.0) ? this_way : that_way;
  };
}

inline Image cond(Region region, Image this_way, Image that_way) {
  return lift([](bool c, Color a, Color b) { return c ? a : b; },
              std::move(region), std::move(this_way), std::move(that_way));
}

inline Image lerp(Blend blend, Image this_way, Image that_way) {
  return lift([](Fraction fraction, Color a, Color b) { return a.weighted_mean(b, fraction); },
              std::move(blend), std::move(this_way), std::move(that_way));
}

inline Image darken(Image image, Blend blend) {
  return [=](const Point p) {
    return image(p).weighted_mean(Colors::black, blend(p));
  };
}

inline Image lighten(Image image, Blend blend) {
  return [=](const Point p) {
    return image(p).weighted_mean(Colors::white, blend(p));
  };
}

#endif //BMP_IMAGE_H
