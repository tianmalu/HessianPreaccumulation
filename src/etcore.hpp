#pragma once
#include <tuple>
#include <vector>
#include <ostream>
#include <type_traits>
#include "etheader.hpp"
#include "etconcepts.hpp"
namespace et
{
class ExprBase {};

template <typename OP, typename ... ARGS>
class Expression : ExprBase {
public:
  using Op = OP;
  Expression(ARGS const& ... x) : x_(x...) { }
  decltype(auto) x() const { return x_; }
  decltype(auto) tangent_vector() const { return tangent_vector_; }
  void setTangent(auto&& tangent_vector) const { tangent_vector_= tangent_vector;}
protected:
  const std::tuple<ARGS ...> x_;
  mutable std::vector<double> tangent_vector_{};
};

template <any_expr X>
std::ostream& operator<<(std::ostream& ofs, X&& x) {
  ofs << ComputeValue(x);
  return ofs;
}

template <typename X_T, typename Y_T> requires any_is_expr_or_leaf<X_T,Y_T> auto operator+(X_T&& x, Y_T&& y) { return Expression<OpAdd , X_T, Y_T>(x, y); }
template <typename X_T, typename Y_T> requires any_is_expr_or_leaf<X_T,Y_T> auto operator-(X_T&& x, Y_T&& y) { return Expression<OpSub , X_T, Y_T>(x, y); }
template <any_expr_or_leaf X_T>                                             auto operator-(X_T&& x         ) { return Expression<OpUSub, X_T     >(x   ); } //<0
template <typename X_T, typename Y_T> requires any_is_expr_or_leaf<X_T,Y_T> auto operator*(X_T&& x, Y_T&& y) { return Expression<OpMul , X_T, Y_T>(x, y); }
template <any_expr_or_leaf X_T>                                             auto sin      (X_T&& x         ) { return Expression<OpSin , X_T     >(x   ); }
} // namespace et
