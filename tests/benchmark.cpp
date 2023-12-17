#include <benchmark/benchmark.h>
#include <iostream>
//#include "tangent.hpp"
#include "dco.hpp"
#include "et.hpp"

using namespace std;

template <typename T>
  void f(std::vector<T> const& x, std::vector<T> & y) {
    for (std::size_t i = 0; i < x.size(); ++i) {
      y[i] = 0.0;
    }
    for (int j = 0; j< 40; j++) {
      for (std::size_t i = 0; i < x.size(); ++i) {
        y[i] = y[i] + sin(sin(x[i]) * x[i] + 2*x[i]);
      }
    }
}

static void BM_T(benchmark::State& state)
{
  constexpr std::size_t n = 160;
  using type = dco::gt1s<double>::type;
  std::vector<type> x(n, 1.1), y(n), diag(n);
  for (std::size_t i = 0; i < n; ++i) {
    dco::derivative(x[i]) = 1.0;
  }
  while (state.KeepRunning()) {
    f(x, y);
  }
  for (std::size_t i = 0; i < n; ++i) {
    diag[i] = dco::derivative(y[i]);
  }
  benchmark::DoNotOptimize(diag.data());
  std::cout << y[0] << " " << diag[0] << std::endl;
}
BENCHMARK(BM_T);

static void BM_VT(benchmark::State& state)
{
  constexpr std::size_t n = 160, vecsize = 16;
  using type = dco::gt1v<double,vecsize>::type;
  std::vector<type> x(n, 1.1), y(n);
  std::vector<double> diag(n);
  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = 0; j < vecsize; ++j) {
      derivative(x[i])[j] = 1.0;
    }
  }
  while (state.KeepRunning()) {
    f(x, y);
  }
  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = 0; j < vecsize; ++j) {
      diag[i] = derivative(y[i])[j];
    }
  }
  benchmark::DoNotOptimize(diag.data());
  std::cout << y[0] << " " << diag[0] << std::endl;
}
BENCHMARK(BM_VT);

static void BM_VT_ET(benchmark::State& state)
{
  constexpr std::size_t n = 160, vecsize = 16;
  using basetype = dco::gt1v<double,vecsize>::type;
  using type = et::ExprLeaf<basetype>;
  std::vector<type> x(n, 1.1), y(n);
  std::vector<double> diag(n);
  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = 0; j < vecsize; ++j) {
      derivative(x[i])[j] = 1.0;
    }
  }
  while (state.KeepRunning()) {
    f(x, y);
  }
  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = 0; j < vecsize; ++j) {
      diag[i] = derivative(y[i])[j];
    }
  }
  benchmark::DoNotOptimize(diag.data());
  std::cout << y[0] << " " << diag[0] << std::endl;
}
BENCHMARK(BM_VT_ET);

BENCHMARK_MAIN();
