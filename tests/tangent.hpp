#include <iostream>
#include <concepts>
#include <cmath>
#include <vector>

namespace ad {

struct TangentBase {};
struct VTangentBase {};

template <typename T>
concept any_stangent = std::is_base_of_v<TangentBase, std::remove_reference_t<T>>;
template <typename T>
concept any_vtangent = std::is_base_of_v<VTangentBase, std::remove_reference_t<T>>;
template <typename T>
concept any_tangent = any_stangent<T> || any_vtangent<T>;
template <typename T> concept has_function_v = requires(T x) { {x.v()}; };
template <typename T> concept has_function_d = requires(T x) { {x.d()}; };

template <typename T, typename S>
concept any_is_stangent = (any_stangent<T> || any_stangent<S>);
template <typename T, typename S>
concept any_is_vtangent = (any_vtangent<T> || any_vtangent<S>);
template <typename T, typename S>
concept any_is_tangent = (any_is_stangent<T,S> || any_is_vtangent<T,S>);

template <typename X_T>
decltype(auto) value(X_T& x) {
  if constexpr (has_function_v<X_T>) {
    return x.v();
  } else {
    return x;
  }
}
template <typename X_T>
decltype(auto) derivative(X_T& x) {
  if constexpr (has_function_d<X_T>) {
    return x.d();
  } else {
    return 0.0;
  }
}

template <typename T, int VSIZE>
class VectorTangent : VTangentBase {
  T v_{0.0};
  std::vector<T> d_=std::vector<T>(VSIZE,0.0);
public:
  VectorTangent() = default;
  VectorTangent(VectorTangent const&) = default;
  VectorTangent(VectorTangent &&) = default;
  VectorTangent& operator=(VectorTangent const&) = default;
  VectorTangent& operator=(VectorTangent &&) = default;

  VectorTangent(T const& x) : v_{x} {}
  VectorTangent(T const& x, std::vector<T> const& d) : v_{x}, d_{d} {}
  VectorTangent& operator=(T const& x) { v_=x; d_=std::vector<T>(VSIZE,0.0); return *this; }

  T& v() { return v_; }
  const T& v() const { return v_; }
  T& d(size_t i) { return d_[i]; }
  const T& d(size_t i) const { return d_[i]; }
  std::vector<T>& d() { return d_; }
  const std::vector<T>& d() const { return d_; }

  VectorTangent operator+() const { return *this; }
  VectorTangent operator-() const {
    std::vector<T> vt(VSIZE,0.0);
    for (size_t i=0; i<VSIZE; i++) {
      vt[i] = -d_[i];
    }
    return {-v_,vt};
  }
};

template<typename T>
class Tangent : TangentBase {
  T v_=0.0, d_=0.0;
public:
  using basetype = T;
  Tangent() = default;
  Tangent(Tangent const&) = default;
  Tangent(Tangent &&) = default;
  Tangent& operator=(Tangent const&) = default;
  Tangent& operator=(Tangent &&) = default;

  Tangent(T const& x) : v_{x}, d_{0.0} {}
  Tangent& operator=(T const& x) { v_=x; d_=0.0; return *this; }
  Tangent(T const& v, T const& d) : v_(v), d_(d) {}

  T& v() { return v_; }
  const T& v() const { return v_; }
  T& d() { return d_; }
  const T& d() const { return d_; }

  Tangent operator+() const { return *this; }
  Tangent operator-() const { return {-v_,-d_}; }
};

template<typename T, typename S> requires any_is_stangent<T,S>
auto operator+(T && x, S && y)
  -> std::common_type_t<std::remove_cvref_t<T>, std::remove_cvref_t<S>>
{
  return {value(x)+value(y), derivative(x)+derivative(y)};
}

template<typename T, typename S> requires any_is_vtangent<T,S>
auto operator+(T && x, S && y)
  -> std::common_type_t<std::remove_cvref_t<T>, std::remove_cvref_t<S>>
{
  std::remove_cvref_t<decltype(derivative(x))> vt(derivative(x).size(),0.0);
  for (size_t i=0; i<vt.size(); i++) {
    vt[i] = derivative(x)[i] + derivative(y)[i];
  }
  return {value(x)+value(y), vt};
}

template<typename T, typename S> requires any_is_tangent<T,S>
auto operator-(T && x, S && y)
{
  return x+(-y);
}

template<typename T, typename S> requires any_is_stangent<T,S>
auto operator*(T && x, S && y)
  -> std::common_type_t<std::remove_cvref_t<T>, std::remove_cvref_t<S>>
{
  if constexpr(any_stangent<T> && any_stangent<S>) {
    return {value(x)*value(y), value(y)*derivative(x)+value(x)*derivative(y)};
  } else if constexpr (any_stangent<T>) {
    return {value(x)*value(y), value(y)*derivative(x)};
  } else {
    return {value(x)*value(y), value(x)*derivative(y)};
  }
}

template<typename T, typename S> requires any_is_vtangent<T,S>
auto operator*(T && x, S && y)
  -> std::common_type_t<std::remove_cvref_t<T>, std::remove_cvref_t<S>>
{
  if constexpr(any_vtangent<T> && any_vtangent<S>) {
    std::remove_cvref_t<decltype(derivative(x))> vt(derivative(x).size(),0.0);
    for (size_t i=0; i<vt.size(); i++) {
      vt[i] += value(y)*derivative(x)[i] + value(x)*derivative(y)[i];
    }
    return {value(x)*value(y), vt};
  } else if constexpr (any_vtangent<T>) {
    std::remove_cvref_t<decltype(derivative(x))> vt(derivative(x).size(),0.0);
    for (size_t i=0; i<vt.size(); i++) {
      vt[i] += value(y)*derivative(x)[i];
    }
    return {value(x)*value(y), vt};
  } else {
    std::remove_cvref_t<decltype(derivative(y))> vt(derivative(y).size(),0.0);
    for (size_t i=0; i<vt.size(); i++) {
      vt[i] += value(x)*derivative(y)[i];
    }
    return {value(x)*value(y), vt};
  }
}

template<typename T>
ad::Tangent<T> sin(ad::Tangent<T> const& x)
{
  return {std::sin(x.v()), derivative(x)*std::cos(x.v())};
}

template<typename T,int vsize>
VectorTangent<T,vsize> sin(VectorTangent<T,vsize> const& x)
{
  VectorTangent<T,vsize> vt;
  vt.v() = std::sin(x.v());
  auto pd = std::cos(x.v());
  for (int i=0; i<vsize; i++) {
    vt.d(i) = x.d(i)*pd;
  }
  return vt;
}

template<typename T>
std::ostream& operator<<(std::ostream& ofs, const Tangent<T>& x)
{
  return ofs << "v:" << x.v() << ", d:" << x.d();
}
template<typename T, int VSIZE>
std::ostream& operator<<(std::ostream& ofs, const VectorTangent<T,VSIZE>& x)
{
  ofs << "v:" << x.v() << ", d:";
  for (auto& xi : x.d()) ofs << xi << " ";
  return ofs;
}
}
