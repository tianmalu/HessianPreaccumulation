#pragma once
#include <concepts>
#include <type_traits>
#include <functional>
#include "etheader.hpp"
namespace et {

template <typename T>
concept any_expr = std::is_base_of_v<ExprBase, std::remove_reference_t<T>>;
template <typename T>
concept any_leaf = std::is_base_of_v<ExprLeafBase, std::remove_reference_t<T>>;
template <typename T>
concept any_expr_or_leaf = any_expr<T> || any_leaf<T>;
template <typename T>
concept neither_expr_nor_leaf = !any_expr_or_leaf<T>;

template <typename T, typename S>
concept any_is_expr_or_leaf = any_expr_or_leaf<T> || any_expr_or_leaf<S>;
template <typename T, typename S>
concept any_is_leaf = any_leaf<T> || any_leaf<S>;

//template <typename T, typename S>
//using value_type_of_t = typename std::common_type_t<std::remove_cvref_t<T>, std::remove_cvref_t<S>>::value_t;

template <typename T> concept has_function_v = requires(T x) {{x.v()};};
template <typename T> concept has_function_d = requires(T x) {{x.d()};};
template <typename T> concept has_vecsize = requires(T x) {{x.vecsize};};

template <typename V_T, typename T> concept has_constructor_with = requires(T x) {{V_T(x)};};
} // namespace et
