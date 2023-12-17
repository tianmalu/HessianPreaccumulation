#pragma once
#include <tuple>
#include <functional>
#include "etconcepts.hpp"
#include <vector>
namespace et {
class ETEvaluatorBase{};

template<typename LE, typename LV, typename LP>
class ETEvaluator : ETEvaluatorBase {
  LE const& lambda_eval;
  LV const& lambda_leaf;
  LP const& lambda_passive;
  
public:
  ETEvaluator() = delete;
  ETEvaluator(ETEvaluator const&) = delete;
  ETEvaluator(ETEvaluator&&) = delete;
  ETEvaluator operator=(ETEvaluator const&) = delete;
  ETEvaluator operator=(ETEvaluator&&) = delete;
  ETEvaluator(LE const& le, LV const& ll, LP const& lp)
    : lambda_eval(le), lambda_leaf(ll), lambda_passive(lp) {}

  template <typename T>
  decltype(auto) operator() (T const& x) const {
    return operator()(x, 0.0, 0.0);
  }

  // x:node 
  template <typename T>
  decltype(auto) operator() (T const& x, auto&& lhs, auto&& data) const {
    if constexpr (any_expr<T>)
      return lambda_eval(typename T::Op(), x, lhs, data);
    else if constexpr (any_leaf<T>)
      return lambda_leaf(x, lhs, data);
    else
      return lambda_passive(x);
  }
  //data: adjoint data2: adjoint of tangent
  template <typename T>
  decltype(auto) operator() (T const& x, auto&& lhs, auto&& data, auto&& data2, auto&& offset) const {
    if constexpr (any_expr<T>)
      return lambda_eval(typename T::Op(), x, lhs, data, data2, offset);
    else if constexpr (any_leaf<T>)
      return lambda_leaf(x, lhs, data, data2);
    else
      return lambda_passive(x);
  }
};

ETEvaluator ComputeActiveValue {
  [](auto&& op, auto&& x, auto&&, auto&&) { return std::apply(op.aeval,x.x()); },
  [](           auto&& x, auto&&, auto&&) { return x.base();              },
  [](           auto&& x                ) { return x;                      }
};

ETEvaluator ComputeValue {
  [](auto&& op, auto&& x, auto&&, auto&&) { return std::apply(op.eval,x.x()); },
  [](           auto&& x, auto&&, auto&&) { return ad::passive_value(x.base());  },
  [](           auto&& x                ) { return x;                     }
};

ETEvaluator ComputeVectorTangent {
  [](auto&& op, auto&& x, auto&&, auto&&) { x.setTangent(std::apply(op.tangent_eval,x.x())); return x.tangent_vector(); },
  [](           auto&& x, auto&&, auto&&) { 
    return std::vector<double>{1.0};
  },
  [](           auto&& x                ) { return std::vector<double>{};                     }
};

ETEvaluator ComputeSLRGradient {
  [](auto&& op, auto&& x, auto&& lhs, auto&& grad) 
  { 
  auto res= std::apply(op.deval,std::tuple_cat(x.x(), std::forward_as_tuple(lhs, grad))); 
  return res; },

  [](           auto&& x, auto&& lhs, auto&& grad) {
    derivative(lhs) += derivative(x)*grad;
    return ad::passive_value(x.base());
  },
  [](           auto&& x                          ) { return x;  }
};

ETEvaluator ComputeSLRHessian {
  [](auto&& op, auto&& x, auto&& lhs, auto&& grad, auto&& grad2, auto&& offset)
  {
  auto res= std::apply(op.d2eval,std::tuple_cat(x.x(), std::forward_as_tuple(lhs, grad, grad2, offset)));
  return res;
  },

  [](           auto&& x, auto&& lhs, auto&& grad, auto&& grad2) {
    std::cout<<"LEAF: "<<ad::passive_value(x.base())<<"  HESSIAN: ";
    for (auto gi : grad2) {
      std:: cout << gi << " ";
    }
    std::cout << std::endl;
    return ad::passive_value(x.base());
  },
  [](           auto&& x   ) { return x; }
};
} // namespace et
