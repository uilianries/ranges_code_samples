#pragma once

#ifndef USE_RANGE_V3
////////////////////////////////////////////////////////////////////////////////
// CPP_fun
// Usage:
//   template <typename A, typename B>
//   void CPP_fun(foo)(A a, B b)([const]opt [noexcept(true)]opt
//       requires Concept1<A> && Concept2<B>)
//   {}
//
// Note: This macro cannot be used when the last function argument is a
//       parameter pack.
#define CPP_PP_EXPAND(...) __VA_ARGS__
#define CPP_FUN_IMPL_1_(...) (__VA_ARGS__) CPP_PP_EXPAND /**/
#define CPP_fun(X) X CPP_FUN_IMPL_1_

#ifdef USE_STL2
#include <experimental/ranges/algorithm>
#include <experimental/ranges/ranges>
#include <vector>

namespace ranges = std::experimental::ranges;

STL2_OPEN_NAMESPACE {
  template <range R> using range_value_t = ext::range_value_t<R>;

  inline constexpr auto to_vector = [](auto &&rng) {
    using Cont  = std::vector<range_value_t<decltype(rng)>>;
    auto common = std::forward<decltype(rng)>(rng) | views::common;
    return Cont{begin(common), end(common)};
  };

  template <view V> auto operator|(V &&v, decltype(to_vector) tv) {
    return tv(std::forward<V>(v));
  }

  struct unwrap_reference_fn {
    template <typename T> T &&operator()(T &&t) const noexcept {
      return static_cast<T &&>(t);
    }

    template <typename T>
    typename reference_wrapper<T>::reference
      operator()(reference_wrapper<T> t) const noexcept {
      return t.get();
    }

    template <typename T>
    T &operator()(std::reference_wrapper<T> t) const noexcept {
      return t.get();
    }

    template <typename T> T &operator()(ref_view<T> t) const noexcept {
      return t.base();
    }
  };

  inline constexpr auto unwrap_reference = unwrap_reference_fn{};

  namespace views {

  namespace detail {
  struct c_str_fn {
    template <typename Char, size_t N> auto operator()(Char (&sz)[N]) const {
      return subrange{&sz[0], &sz[N - 1]};
    }
  };
  } // namespace detail

  inline constexpr detail::c_str_fn c_str;

  inline constexpr auto zip = [](auto &&rng1, auto &&rng2) {
    std::vector<
      std::pair<range_value_t<decltype(rng1)>, range_value_t<decltype(rng2)>>>
      v;
    ranges::transform(std::forward<decltype(rng1)>(rng1),
                      std::forward<decltype(rng2)>(rng2), back_inserter(v),
                      [](auto &&val1, auto &&val2) {
                        return std::pair{std::forward<decltype(val1)>(val1),
                                         std::forward<decltype(val2)>(val2)};
                      });
    return v;
  };

  using namespace ext;

  } // namespace views
}
STL2_CLOSE_NAMESPACE

#endif // USE_STL2

#endif // USE_RANGE_V3
