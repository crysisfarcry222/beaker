// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include <beaker/numeric/generation/gen.hpp>
#include <beaker/numeric/type.hpp>
#include <beaker/numeric/expr.hpp>


namespace beaker {
namespace numeric {


/// Generate an integer type. If the precision is greater than 128, the type
/// will be indirect.
template<typename T>
static inline cg::type
generate_int_type(generator& g, const T& t)
{
  int prec = t.get_precision();
  bool ref = (prec <= 128);
  return {llvm::Type::getIntNTy(g.get_context(), prec), ref};
}

static cg::type
generate_fp_type(generator& g, const type& t)
{
  int prec = cast<float_type>(t).get_precision();
  if (prec == 16)
    return llvm::Type::getHalfTy(g.get_context());
  else if (prec == 32)
    return llvm::Type::getFloatTy(g.get_context());
  else if (prec == 64)
    return llvm::Type::getDoubleTy(g.get_context());
  else
    assert(false && "invalid floating point precision");
}

// Generate a numeric type.
cg::type
gen_algo::operator()(generator& gen, const type& t) const
{
  switch (t.get_kind()) {
    case char_type_kind:
      return generate_int_type(gen, cast<char_type>(t));
    case nat_type_kind:
      return generate_int_type(gen, cast<nat_type>(t));
    case int_type_kind:
      return generate_int_type(gen, cast<int_type>(t));
    case mod_type_kind:
      return generate_int_type(gen, cast<mod_type>(t));
    case float_type_kind:
      return generate_fp_type(gen, cast<float_type>(t));
    default:
      break;
  }
  assert(false && "not a numeric type");
}

/// Generate an integer literal.
///
/// FIXME: Should we really handle integers > 64/128 bits in the language?
/// Probably not.
static cg::value 
generate_int_expr(generator& gen, const int_expr& e)
{
  int prec = get_precision(e.get_type());
  
  llvm::Builder ir(gen.get_current_block());
  cg::value val = ir.getIntN(prec, e.get_value().get_integer());
  cg::type type = generate(gen, e.get_type());
  if (type.is_direct()) {
    return val;
  } else {
    // Large integers are passed indirectly, so allocate the temporary, store
    // the computed value, and return the pointer.
    //
    // Copy ellision: don't create a new temporary we're initializing an
    // existing object.
    //
    // TODO: Factor this out so we can reuse it everywhere.
    cg::value ptr = gen.get_initialized_object();
    if (!ptr) {
      llvm::Builder entry(gen.get_entry_block());
      ptr = entry.CreateAlloca(type, nullptr, "temp");
    }
    ir.CreateStore(val, ptr);
    return ptr;
  }
}

/// Generate a floating point literal.
static cg::value 
generate_float_expr(generator& gen, const float_expr& e)
{
  cg::type type = generate(gen, e.get_type());
  llvm::Builder ir(gen.get_current_block());
  return llvm::ConstantFP::get(type, e.get_value().get_float());
}

/// Generate an equality comparison.
static cg::value
generate_eq_expr(generator& gen, const eq_expr& e)
{
  cg::value v1 = generate(gen, e.get_lhs());
  cg::value v2 = generate(gen, e.get_rhs());

  llvm::Builder ir(gen.get_current_block());
  const type& t = e.get_lhs().get_type();
  switch (t.get_kind()) {
    case nat_type_kind:
    case int_type_kind:
    case mod_type_kind: {
      cg::type type = generate(gen, t);
      if (type.is_indirect()) {
        // FIXME: This is kind of dumb (and probably REALLY inefficient).
        // Bigint support should probably be provided by a separate module,
        // if not by a user-defined type.
        v1 = ir.CreateLoad(v1);
        v2 = ir.CreateLoad(v2);
      }
      return ir.CreateICmpEQ(v1, v2);
    }
    case float_type_kind:
      return ir.CreateFCmpOEQ(v1, v2);
    default:
      break;
  }
  assert(false && "not a numeric type");
}

/// Generate a not-equals comparison.
static cg::value
generate_ne_expr(generator& gen, const ne_expr& e)
{
  cg::value v1 = generate(gen, e.get_lhs());
  cg::value v2 = generate(gen, e.get_rhs());

  llvm::Builder ir(gen.get_current_block());
  const type& t = e.get_lhs().get_type();
  switch (t.get_kind()) {
    case nat_type_kind:
    case int_type_kind:
    case mod_type_kind:
      return ir.CreateICmpNE(v1, v2);
    case float_type_kind:
      return ir.CreateFCmpONE(v1, v2);
    default:
      break;
  }
  assert(false && "not a numeric type");
}

static cg::value
generate_lt_expr(generator& gen, const lt_expr& e)
{
  cg::value v1 = generate(gen, e.get_lhs());
  cg::value v2 = generate(gen, e.get_rhs());

  llvm::Builder ir(gen.get_current_block());
  const type& t = e.get_lhs().get_type();
  switch (t.get_kind()) {
    case nat_type_kind:
    case mod_type_kind:
      return ir.CreateICmpULT(v1, v2);
    case int_type_kind:
      return ir.CreateICmpSLT(v1, v2);
    case float_type_kind:
      return ir.CreateFCmpOLT(v1, v2);
    default:
      break;
  }
  assert(false && "not a numeric type");
}

static cg::value
generate_gt_expr(generator& gen, const gt_expr& e)
{
  cg::value v1 = generate(gen, e.get_lhs());
  cg::value v2 = generate(gen, e.get_rhs());

  llvm::Builder ir(gen.get_current_block());
  const type& t = e.get_lhs().get_type();
  switch (t.get_kind()) {
    case nat_type_kind:
    case mod_type_kind:
      return ir.CreateICmpUGT(v1, v2);
    case int_type_kind:
      return ir.CreateICmpSGT(v1, v2);
    case float_type_kind:
      return ir.CreateFCmpOGT(v1, v2);
    default:
      break;
  }
  assert(false && "not a numeric type");
}

static cg::value
generate_le_expr(generator& gen, const le_expr& e)
{
  cg::value v1 = generate(gen, e.get_lhs());
  cg::value v2 = generate(gen, e.get_rhs());

  llvm::Builder ir(gen.get_current_block());
  const type& t = e.get_lhs().get_type();
  switch (t.get_kind()) {
    case nat_type_kind:
    case mod_type_kind:
      return ir.CreateICmpULE(v1, v2);
    case int_type_kind:
      return ir.CreateICmpSLE(v1, v2);
    case float_type_kind:
      return ir.CreateFCmpOLE(v1, v2);
    default:
      break;
  }
  assert(false && "not a numeric type");
}

static cg::value
generate_ge_expr(generator& gen, const ge_expr& e)
{
  cg::value v1 = generate(gen, e.get_lhs());
  cg::value v2 = generate(gen, e.get_rhs());

  llvm::Builder ir(gen.get_current_block());
  const type& t = e.get_lhs().get_type();
  switch (t.get_kind()) {
    case nat_type_kind:
    case mod_type_kind:
      return ir.CreateICmpUGE(v1, v2);
    case int_type_kind:
      return ir.CreateICmpSGE(v1, v2);
    case float_type_kind:
      return ir.CreateFCmpOGE(v1, v2);
    default:
      break;
  }
  assert(false && "not a numeric type");
}

/// Generate an add expression.
static cg::value
generate_add_expr(generator& gen, const add_expr& e)
{
  cg::value v1 = generate(gen, e.get_lhs());
  cg::value v2 = generate(gen, e.get_rhs());

  llvm::Builder ir(gen.get_current_block());
  const type& t = e.get_lhs().get_type();
  switch (t.get_kind()) {
    case nat_type_kind:
      return ir.CreateNUWAdd(v1, v2);
    case int_type_kind:
      return ir.CreateNSWAdd(v1, v2);
    case mod_type_kind:
      return ir.CreateAdd(v1, v2);
    case float_type_kind:
      return ir.CreateFAdd(v1, v2, "");
    default:
      break;
  }
  assert(false && "not a numeric type");
}

static cg::value
generate_sub_expr(generator& gen, const sub_expr& e)
{
  cg::value v1 = generate(gen, e.get_lhs());
  cg::value v2 = generate(gen, e.get_rhs());

  llvm::Builder ir(gen.get_current_block());
  const type& t = e.get_lhs().get_type();
  switch (t.get_kind()) {
    case nat_type_kind:
      return ir.CreateNUWSub(v1, v2);
    case int_type_kind:
      return ir.CreateNSWSub(v1, v2);
    case mod_type_kind:
      return ir.CreateSub(v1, v2);
    case float_type_kind:
      return ir.CreateFSub(v1, v2, "");
    default:
      break;
  }
  assert(false && "not a numeric type");
}

static cg::value
generate_mul_expr(generator& gen, const mul_expr& e)
{
  cg::value v1 = generate(gen, e.get_lhs());
  cg::value v2 = generate(gen, e.get_rhs());

  llvm::Builder ir(gen.get_current_block());
  const type& t = e.get_lhs().get_type();
  switch (t.get_kind()) {
    case nat_type_kind:
      return ir.CreateNUWMul(v1, v2);
    case int_type_kind:
      return ir.CreateNSWMul(v1, v2);
    case mod_type_kind:
      return ir.CreateMul(v1, v2);
    case float_type_kind:
      return ir.CreateFMul(v1, v2, "");
    default:
      break;
  }
  assert(false && "not a numeric type");
}

static cg::value
generate_div_expr(generator& gen, const div_expr& e)
{
  cg::value v1 = generate(gen, e.get_lhs());
  cg::value v2 = generate(gen, e.get_rhs());

  llvm::Builder ir(gen.get_current_block());
  const type& t = e.get_lhs().get_type();
  switch (t.get_kind()) {
    case nat_type_kind:
    case mod_type_kind:
      return ir.CreateUDiv(v1, v2);
    case int_type_kind:
      return ir.CreateSDiv(v1, v2);
    case float_type_kind:
      return ir.CreateFDiv(v1, v2, "");
    default:
      break;
  }
  assert(false && "not a numeric type");
}

static cg::value
generate_rem_expr(generator& gen, const rem_expr& e)
{
  cg::value v1 = generate(gen, e.get_lhs());
  cg::value v2 = generate(gen, e.get_rhs());

  llvm::Builder ir(gen.get_current_block());
  const type& t = e.get_lhs().get_type();
  switch (t.get_kind()) {
    case nat_type_kind:
    case mod_type_kind:
      return ir.CreateURem(v1, v2);
    case int_type_kind:
      return ir.CreateSRem(v1, v2);
    default:
      break;
  }
  assert(false && "not an integral type");
}

static cg::value
generate_neg_expr(generator& gen, const neg_expr& e)
{
  assert(false && "not implemented");
}

static cg::value
generate_rec_expr(generator& gen, const rec_expr& e)
{
  assert(false && "not implemented");
}

// Generate code for the numeric expression e.
cg::value
gen_algo::operator()(generator& gen, const expr& e) const
{
  switch (e.get_kind()) {
#define def_expr(E) \
    case E ## _expr_kind: \
      return generate_ ## E ## _expr(gen, cast<E ## _expr>(e));
#include "../expr.def"
    default:
      break;
  }
  assert(false && "not a numeric expression");
}

} // namespace numeric
} // namespace beaker

