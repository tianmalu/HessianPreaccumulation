#include <iostream>
#include <concepts>
#include <cmath>

namespace interval
{

struct IntervalBase {};

//template <typename T>
//concept any_interval = std::is_base_of_v<IntervalBase, std::remove_reference_t<T>>;
//alternative:
template <typename T>
concept any_interval = requires(T v)
{
  {v.l()} -> std::convertible_to<double>;
  {v.u()};
};

template <typename T, typename S>
concept any_is_interval = (any_interval<T> && any_interval<S>)
    || (any_interval<T> && std::is_convertible_v<S,typename T::basetype>)
    || (any_interval<S> && std::is_convertible_v<T,typename S::basetype>);

template <typename X_T>
decltype(auto) lower(X_T& x) {
  if constexpr (any_interval<X_T>) {
    return x.l();
  } else {
    return x;
  }
}
template <typename X_T>
decltype(auto) upper(X_T& x) {
  if constexpr (any_interval<X_T>) {
    return x.u();
  } else {
    return x;
  }
}

template<typename T>
class Interval : IntervalBase {
  T xl_=0.0, xu_=0.0;
public:
  using basetype = T;
  Interval() = default;
  Interval(Interval const&) = default;
  Interval(Interval &&) = default;
  Interval& operator=(Interval const&) = default;
  Interval& operator=(Interval &&) = default;

  Interval(T const& x) : xl_(x), xu_(x) {}
  Interval(T const& xl, T const& xu) : xl_(xl), xu_(xu) {}
  Interval& operator=(T const& x) { xl_=x; xu_=x; }

  T& l() { return xl_; }
  const T& l() const { return xl_; }
  T& u() { return xu_; }
  const T& u() const { return xu_; }

  Interval operator+() const { return *this; }
  Interval operator-() const { return {-xu_,-xl_}; }
};

template<typename T, typename S> requires any_is_interval<T,S>
auto operator+(T && x, S && y)
  -> std::common_type_t<std::remove_cvref_t<T>, std::remove_cvref_t<S>>
{
  return {lower(x)+lower(y), upper(x)+upper(y)};
}

template<typename T, typename S> requires any_is_interval<T,S>
auto operator-(T && x, S && y)
{
  return x+(-y);
}

template<typename T, typename S> requires any_is_interval<T,S>
auto max(T && x, S && y)
-> std::common_type_t<std::remove_cvref_t<T>, std::remove_cvref_t<S>>
{
  return {std::max(lower(x),lower(y)), std::max(upper(x),upper(y))};
}

template<typename T>
std::ostream& operator<<(std::ostream& ofs, const Interval<T>& x)
{
  return ofs << "[" << x.l() << "," << x.u() << "]";
}
}
