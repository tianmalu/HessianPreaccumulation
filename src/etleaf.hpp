#pragma once
#include <ostream>
#include "etconcepts.hpp"
#include <vector>

namespace et
{
class ExprLeafBase { };

template<typename BASE>
class ExprLeaf : ExprLeafBase {
public:
  using base_t = std::remove_cvref_t<BASE>;
protected:
  base_t _base;
public:
  ExprLeaf() = default;
  ExprLeaf(ExprLeaf const&) = default;
  ExprLeaf(ExprLeaf &&) = default;
  ExprLeaf& operator=(ExprLeaf const&) = default;
  ExprLeaf& operator=(ExprLeaf &&) = default;

  template <typename T> requires has_constructor_with<BASE, T> ExprLeaf(T const& x) : _base(x) {}
  template <typename T> requires has_constructor_with<BASE, T>
  ExprLeaf& operator=(T const& x) { _base = x; return *this; }

  ExprLeaf(BASE const& x) : _base(x) {}

  template<any_expr EXPR>
  ExprLeaf(EXPR const& expr) {
    ExprLeaf res;

    std::cout << "Read vector tangent information in nodes:\n";
    auto vt = ComputeVectorTangent(expr);
    for (auto& vti: vt) std::cout<<"v: "<<vti << std::endl;

    //std::cout << "Compute scalar adjoint information:\n";
    //_base = ComputeSLRGradient(expr,res,1.0);
    //derivative(*this) = derivative(res);

    std::cout << "Compute Hessian information:\n";
    std::vector<double> grad2(vt.size(), 0.0);
    std::vector<BASE> arr(vt.size(), 0.0);
    _base= ComputeSLRHessian(expr,arr,1.0,grad2,0);
  }

  template<any_expr EXPR>
  ExprLeaf& operator=(EXPR const& expr) {
    ExprLeaf res;
    _base = ComputeSLRGradient(expr, res, 1.0);
    derivative(*this) = derivative(res);
    return *this;
  }

  decltype(auto) tangent_vector() const { return std::vector<double>{1}; }

  //Type cast
  template <std::integral T>
  explicit operator T() { return static_cast<T>(_base); }
  //return pointer to _base
  base_t      & base()       { return _base; }
  base_t const& base() const { return _base; }
  //operator overloading
  template <any_expr_or_leaf T> ExprLeaf& operator+=(T&& other) { *this = *this + other; return *this;}
  template <any_expr_or_leaf T> ExprLeaf& operator-=(T&& other) { *this = *this + other; return *this;}
  template <any_expr_or_leaf T> ExprLeaf& operator*=(T&& other) { *this = *this * other; return *this;}
  template <any_expr_or_leaf T> ExprLeaf& operator/=(T&& other) { *this = *this / other; return *this;}
};


//GET
template <any_leaf T>
decltype(auto) value(T& x) {
  return ad::value(x.base());
}

template <any_leaf T>
decltype(auto) derivative(T& x) {
  return ad::derivative(x.base());
}

//operator overloading
template <any_leaf T>
std::ostream& operator<<(std::ostream& ofs, T&& x) {
  ofs << x.base();
  return ofs;
}

template <any_leaf T>
std::ifstream& operator>>(std::ifstream& ifs, T& x) {
  ifs >> x.base();
  return ifs;
}

} // namespace et
