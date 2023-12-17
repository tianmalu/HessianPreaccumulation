#pragma once
namespace et {
class ExprBase;
class ExprLeafBase;
class ETEvaluatorBase;

template <typename OP, typename ... ARGS>
class Expression;

struct OpBase;
struct OpUn;
struct OpBin;

struct OpAdd;
struct OpUAdd;
struct OpSub;
struct OpUSub;
struct OpMul;
struct OpDiv;
struct OpSqr;
struct OpInv;
struct OpExp;
struct OpLog;
struct OpCos;
struct OpSin;
struct OpTan;
struct OpAsin;
struct OpAcos;
struct OpAtan;
struct OpAbs;
struct OpFabs;
struct OpSqrt;
struct OpXlog;
struct OpArh;
struct OpErf;
struct OpErfc;
struct OpFstp;
struct OpBstp;
struct OpPow;
struct OpCheb;
struct OpMin;
struct OpMax;
struct OpMono;
} // namespace et
