#ifndef __FUNCTIONAL_H__
#define __FUNCTIONAL_H__

#include <functional>

inline auto compose() {
  return [](const auto arg) {
    return arg;
  };
}

template <typename T, typename ...Args>
auto compose(T t, Args... args) {
  return [=](const auto x) {
    return compose(args...)(t(x));
  };
}

template <typename T, typename ...Args>
auto lift(T f, Args ...args) {
  return [=](const auto x) {
    return f(args(x)...);
  };
}

#endif // __FUNCTIONAL_H__