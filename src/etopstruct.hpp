#pragma once
#include <cmath>
#include <vector>

namespace et {
struct OpUAdd {
  static constexpr auto count=[](auto &&x) {return +CountLeaves(x);};
  static constexpr auto partial(auto &&) { return 1; };
  static constexpr auto tangent_eval=[](auto &&x) { 
    return ComputeVectorTangent(x); 
  };
  static constexpr auto aeval=[](auto &&x){ return +ComputeActiveValue(x); };
  static constexpr auto  eval=[](auto &&x){ return +ComputeValue(x); };
  static constexpr auto deval=[](auto &&x, auto && lhs, auto && grad){
    return ComputeSLRGradient(x,lhs,grad);
  };
  static constexpr auto d2eval=[](auto &&x, auto && lhs, auto && grad, auto && grad2, auto && offset){
    return ComputeSLRHessian(x, lhs, grad, grad2, offset);
  };
};
struct OpAdd {
  static constexpr auto count=[](auto &&x, auto && y) {return CountLeaves(x)+CountLeaves(y);};
  static constexpr auto partial(auto &&) { return 1; };
  static constexpr auto tangent_eval=[](auto &&x, auto &&y) { 
    auto x_vt = ComputeVectorTangent(x);
    auto y_vt = ComputeVectorTangent(y);
    std::vector<double> res(x_vt.size() + y_vt.size(), 0.0);
    for (int i=0; i<x_vt.size(); ++i) {
      res[i] = x_vt[i];
    }
    for (int i=0; i<y_vt.size(); ++i) {
      res[i+x_vt.size()] = y_vt[i];
    }
    return res; 
  };
  static constexpr auto aeval=[](auto &&x, auto && y){ return ComputeActiveValue(x) + ComputeActiveValue(y); };
  static constexpr auto  eval=[](auto &&x, auto && y){ return ComputeValue(x) + ComputeValue(y); };
  static constexpr auto deval=[](auto &&x, auto && y, auto && lhs, auto && grad){
//    ComputeSLRGradient(x,lhs,grad);
//    ComputeSLRGradient(y,lhs,grad);
    return ComputeSLRGradient(x,lhs,grad) + ComputeSLRGradient(y,lhs,grad);
  
  };
  static constexpr auto d2eval=[](auto &&x, auto &&y, auto && lhs, auto && grad, auto && grad2, auto && offset){
    return ComputeSLRHessian(x, lhs, grad, grad2, offset)+ ComputeSLRHessian(y, lhs, grad, grad2, offset+ComputeVectorTangent(x).size());
  };
};

struct OpUSub {
  static constexpr auto count=[](auto &&x) {return CountLeaves(x);};
  static constexpr auto partial(auto &&) { return -1; };
  static constexpr auto tangent_eval=[](auto &&x) { 
    auto x_vt = ComputeVectorTangent(x);
    std::vector<double> res(x_vt.size(), 0.0);
    for (int i=0; i<x_vt.size(); ++i) {
      res[i] = -x_vt[i];
    }
    return res;  
  };
  static constexpr auto aeval=[](auto &&x){ return -ComputeActiveValue(x); };
  static constexpr auto  eval=[](auto &&x){ return -ComputeValue(x); };
  static constexpr auto deval=[](auto &&x, auto && lhs, auto && grad){
    return ComputeSLRGradient(x,lhs,-grad);
  };
  static constexpr auto d2eval=[](auto &&x, auto && lhs, auto && grad, auto && grad2, auto && offset){
    std::vector<double> res(grad2.size(), 0.0);
    for (int i=0; i<grad2.size(); ++i) {
      res[i] = -grad2[i];
    }
    return ComputeSLRHessian(x, lhs, -grad, res, offset);
  };
};
struct OpSub {
  static constexpr auto count=[](auto &&x, auto && y) {return CountLeaves(x)+CountLeaves(y);};
  static constexpr auto partial1(auto &&) { return 1; };
  static constexpr auto partial2(auto &&) { return -1; };
  static constexpr auto tangent_eval=[](auto &&x, auto &&y) { 
    auto x_vt = ComputeVectorTangent(x);
    auto y_vt = ComputeVectorTangent(y);
    std::vector<double> res(x_vt.size() + y_vt.size(), 0.0);
    for (int i=0; i<x_vt.size(); ++i) {
      res[i] = x_vt[i];
    }
    for (int i=0; i<y_vt.size(); ++i) {
      res[i+x_vt.size()] = -y_vt[i];
    }
    return res; 
  };
  static constexpr auto aeval=[](auto &&x, auto && y){ return ComputeActiveValue(x) - ComputeActiveValue(y); };
  static constexpr auto  eval=[](auto &&x, auto && y){ return ComputeValue(x) - ComputeValue(y); };
  static constexpr auto deval=[](auto &&x, auto && y, auto && lhs, auto && grad){
//    ComputeSLRGradient(x,lhs,grad);
//    ComputeSLRGradient(y,lhs,-grad);
    return ComputeSLRGradient(x,lhs,grad) - ComputeSLRGradient(y,lhs,-grad);
  };
  static constexpr auto d2eval=[](auto &&x, auto &&y, auto && lhs, auto && grad, auto && grad2, auto && offset){
    std::vector<double> res(grad2.size(), 0.0);
    for (int i=0; i<grad2.size(); ++i) {
      res[i] = -grad2[i];
    }
    return ComputeSLRHessian(x, lhs, grad, grad2, offset)- ComputeSLRHessian(y, lhs, -grad, res, offset+ComputeVectorTangent(x).size());
    //return ComputeSLRHessian(x, tx, lhs, grad, grad2)- ComputeSLRHessian(y, tx, lhs, grad, -grad2);
  };
};
struct OpMul {
  static constexpr auto count=[](auto &&x, auto && y) { return CountLeaves(x)+CountLeaves(y);};
  static constexpr auto partial1(auto &&x, auto && y) { return ComputeValue(y); };
  static constexpr auto partial2(auto &&x, auto && y) { return ComputeValue(x); };
  static constexpr auto tangent_eval=[](auto &&x, auto &&y) { 
    auto x_vt = ComputeVectorTangent(x);
    auto y_vt = ComputeVectorTangent(y);
    auto x_value = ComputeValue(x);
    auto y_value = ComputeValue(y);
    std::vector<double> res(x_vt.size() + y_vt.size(), 0.0);
    for (int i=0; i<x_vt.size(); ++i) {
      res[i] = x_vt[i]*y_value;
    }
    for (int i=0; i<y_vt.size(); ++i) {
      res[i+x_vt.size()] = y_vt[i]*x_value;
    }
    return res;
    //x(partial1(x, y)*tx); y(partial2(x, y)*tx); return x.tangent_vector()+ y.tangent_vector(); 
  };
  static constexpr auto aeval=[](auto &&x, auto && y){ return ComputeActiveValue(x) * ComputeActiveValue(y); };
  static constexpr auto  eval=[](auto &&x, auto && y){ return ComputeValue(x) * ComputeValue(y); };
  static constexpr auto deval=[](auto &&x, auto && y, auto && lhs, auto && grad){
//    ComputeSLRGradient(x,lhs,grad*ComputeValue(y));
//    ComputeSLRGradient(y,lhs,grad*ComputeValue(x));
    const auto x_val = ComputeValue(x);
    const auto y_val = ComputeValue(y);
    return ComputeSLRGradient(x,lhs,grad*y_val) * ComputeSLRGradient(y,lhs,grad*x_val);
    
  };
  static constexpr auto d2eval=[](auto &&x, auto &&y, auto && lhs, auto && grad, auto && grad2, auto && offset){
    std::vector<double> res_x(grad2.size(), 0.0);
    std::vector<double> res_y(grad2.size(), 0.0);
    auto tx_x = ComputeVectorTangent(x);
    auto tx_y = ComputeVectorTangent(y);
    for (int i=0; i<grad2.size(); ++i) {
      res_x[i] = grad2[i]*partial1(x,y);
      res_y[i] = grad2[i]*partial2(x,y);
    }
    for (int i=0; i<tx_y.size(); ++i) {
      res_x[i+offset+tx_x.size()] += grad*tx_y[i];
    }
    for (int i=0; i<tx_x.size(); ++i) {
      res_y[i+offset] += grad*tx_x[i];
    }
    return ComputeSLRHessian(x, lhs, grad*partial1(x,y), res_x, offset)* ComputeSLRHessian(y, lhs, grad*partial2(x,y), res_y, offset+tx_x.size());
    //return ComputeSLRHessian(x, lhs, grad*partial1(x,y), grad2*partial1(x,y)+ grad*y.tangent_vector())* ComputeSLRHessian(y, lhs, grad*partial2(x,y), grad2*partial2(x,y)+ grad*x.tangent_vector());
  };
};
struct OpSin {
  static constexpr auto count=[](auto &&x) { return CountLeaves(x);};
  static constexpr auto partial(auto &&x) { return std::cos(ComputeValue(x)); };
  static constexpr auto tangent_eval=[](auto &&x) { 
    //x(std::cos(tx)); return x.tangent_vector(); 
    auto x_vt = ComputeVectorTangent(x);
    std::vector<double> res(x_vt.size(), 0.0);
    for (int i=0; i<x_vt.size(); ++i) {
      res[i] = x_vt[i]*partial(x);
    }
    return res;
  };
  static constexpr auto aeval=[](auto &&x){ return std::sin(ComputeActiveValue(x)); };
  static constexpr auto  eval=[](auto &&x){ return std::sin(ComputeValue(x)); };
  static constexpr auto deval=[](auto &&x, auto && lhs, auto && grad){
//    const auto x_val = ComputeValue(x);
    return std::sin(ComputeSLRGradient(x,lhs,grad*std::cos(ComputeValue(x))));
  };
  static constexpr auto d2eval=[](auto &&x, auto && lhs, auto && grad, auto && grad2, auto && offset){
    //return ComputeSLRHessian(x, tx, lhs, grad*partial(x), grad2*partial(x)+ grad*(-std::sin(ComputeValue(x)))*x.tangent_vector());
    std::vector<double> res_x(grad2.size(), 0.0);
    auto tx_x = ComputeVectorTangent(x);
    for (int i=0; i<offset; ++i) {
      res_x[i] = grad2[i]*partial(x);
    }
    for (int i=offset; i<tx_x.size()+offset; ++i) {
      res_x[i] = grad2[i]*partial(x)+ grad*tx_x[i-offset]*(-std::sin(ComputeValue(x)));
    }
    return std::sin(ComputeSLRHessian(x, lhs, grad*partial(x), res_x, offset));
  };
};

} // namespace et
