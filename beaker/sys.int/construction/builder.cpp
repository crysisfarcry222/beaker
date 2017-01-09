// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "builder.hpp"
#include "../type.hpp"
#include "../expr.hpp"

#include <beaker/sys.bool/type.hpp>
#include <beaker/sys.bool/construction/builder.hpp>

#include <beaker/base/module.hpp>


namespace beaker {
namespace sys_int {

static inline sys_bool::builder& 
get_logic_builder(module& m)
{
  return m.get_builder<sys_bool::builder>();
}

builder::builder(module& m)
  : basic_builder<sys_int_lang>(m),
    nat_(&get_language().make_canonical_set<nat_type>()),
    int_(&get_language().make_canonical_set<int_type>()),
    mod_(&get_language().make_canonical_set<mod_type>())
{ }

// Returns true when p is an acceptable integer type.
//
// FIXME: This isn't generally correct. We have to verify that p is one of the
// allowable integer types on the system ('int' could map to int12, for
// example).
static inline bool
check_precision(int p)
{
  switch (p) {
    case 8:
    case 16:
    case 32:
    case 64:
    case 128:
      return true;
    default:
      return false;
  } 
}

/// Returns the canonical type `natp` with `p` bits of precision.
nat_type&
builder::get_nat_type(int p) 
{
  assert(check_precision(p));
  return nat_->get(p);
}

/// Returns the canonical type `intp` with `p` bits of precision.
int_type&
builder::get_int_type(int p) 
{
  assert(check_precision(p));
  return int_->get(p);
}

/// Returns the canonical type `modp` with `p` bits of precision.
mod_type&
builder::get_mod_type(int p) 
{
  assert(check_precision(p));
  return mod_->get(p);
}

int_expr&
builder::make_int_expr(type& t, const value& v)
{
  assert(is_integral_type(t));
  assert(v.is_integer());
  return make<int_expr>(t, v);
}

int_expr&
builder::make_int_expr(type& t, value&& v)
{
  assert(is_integral_type(t));
  assert(v.is_integer());
  return make<int_expr>(t, std::move(v));
}

/// Returns the integer literal `n` with type t. The type of the expression 
/// shall be an integral type.
int_expr& 
builder::make_int_expr(type& t, int n) 
{
  assert(is_integral_type(t));
  return make_int_expr(t, value(n)); 
}

/// Returns the expression `e1 == e2`.
///
/// The operands `e1` and `e2` shall have the same type `t`, and `t` shall be 
/// an arithmetic type. The type of the expression shall be `bool`.
eq_expr&
builder::make_eq_expr(expr& e1, expr& e2)
{
  assert(equivalent(e1.get_type(), e2.get_type()));
  assert(is_integral_expression(e1));
  sys_bool::builder& b = get_logic_builder(get_module());
  return make<eq_expr>(b.get_bool_type(), e1, e2);
}

/// Returns the expression `e1 != e2`.
///
/// The operands `e1` and `e2` shall have the same type `t`, and `t` shall be 
/// an arithmetic type. The type of the expression shall be `bool`.
ne_expr&
builder::make_ne_expr(expr& e1, expr& e2)
{
  assert(equivalent(e1.get_type(), e2.get_type()));
  assert(is_integral_expression(e1));
  sys_bool::builder& b = get_logic_builder(get_module());
  return make<ne_expr>(b.get_bool_type(), e1, e2);
}

/// Returns the expression `e1 < e2`.
///
/// The operands `e1` and `e2` shall have the same type `t`, and `t` shall be 
/// an arithmetic type. The type of the expression shall be `bool`.
lt_expr&
builder::make_lt_expr(expr& e1, expr& e2)
{
  assert(equivalent(e1.get_type(), e2.get_type()));
  assert(is_integral_expression(e1));
  sys_bool::builder& b = get_logic_builder(get_module());
  return make<lt_expr>(b.get_bool_type(), e1, e2);
}

/// Returns the expression `e1 > e2`.
///
/// The operands `e1` and `e2` shall have the same type `t`, and `t` shall be 
/// an arithmetic type. The type of the expression shall be `bool`.
gt_expr&
builder::make_gt_expr(expr& e1, expr& e2)
{
  assert(equivalent(e1.get_type(), e2.get_type()));
  assert(is_integral_expression(e1));
  sys_bool::builder& b = get_logic_builder(get_module());
  return make<gt_expr>(b.get_bool_type(), e1, e2);
}

/// Returns the expression `e1 <= e2`.
///
/// The operands `e1` and `e2` shall have the same type `t`, and `t` shall be 
/// an arithmetic type. The type of the expression shall be `bool`.
le_expr&
builder::make_le_expr(expr& e1, expr& e2)
{
  assert(equivalent(e1.get_type(), e2.get_type()));
  assert(is_integral_expression(e1));
  sys_bool::builder& b = get_logic_builder(get_module());
  return make<le_expr>(b.get_bool_type(), e1, e2);
}

/// Returns the expression `e1 >= e2`.
///
/// The operands `e1` and `e2` shall have the same type `t`, and `t` shall be 
/// an arithmetic type. The type of the expression shall be `bool`.
ge_expr&
builder::make_ge_expr(expr& e1, expr& e2)
{
  assert(equivalent(e1.get_type(), e2.get_type()));
  assert(is_integral_expression(e1));
  sys_bool::builder& b = get_logic_builder(get_module());
  return make<ge_expr>(b.get_bool_type(), e1, e2);
}

add_expr&
builder::make_add_expr(type& t, expr& e1, expr& e2)
{
  assert(equivalent(t, e1.get_type()));
  assert(equivalent(t, e2.get_type()));
  assert(is_integral_type(t));
  return make<add_expr>(t, e1, e2);
}

sub_expr&
builder::make_sub_expr(type& t, expr& e1, expr& e2)
{
  assert(equivalent(t, e1.get_type()));
  assert(equivalent(t, e2.get_type()));
  assert(is_integral_type(t));
  return make<sub_expr>(t, e1, e2);
}

mul_expr&
builder::make_mul_expr(type& t, expr& e1, expr& e2)
{
  assert(equivalent(t, e1.get_type()));
  assert(equivalent(t, e2.get_type()));
  assert(is_integral_type(t));
  return make<mul_expr>(t, e1, e2);
}

div_expr&
builder::make_div_expr(type& t, expr& e1, expr& e2)
{
  assert(equivalent(t, e1.get_type()));
  assert(equivalent(t, e2.get_type()));
  assert(is_integral_type(t));
  return make<div_expr>(t, e1, e2);
}

rem_expr&
builder::make_rem_expr(type& t, expr& e1, expr& e2)
{
  assert(equivalent(t, e1.get_type()));
  assert(equivalent(t, e2.get_type()));
  assert(is_integral_type(t));
  return make<rem_expr>(t, e1, e2);
}

neg_expr&
builder::make_neg_expr(type& t, expr& e)
{
  assert(equivalent(t, e.get_type()));
  assert(is_integral_type(t));
  return make<neg_expr>(t, e);
}

rec_expr&
builder::make_rec_expr(type& t, expr& e)
{
  assert(equivalent(t, e.get_type()));
  assert(is_integral_type(t));
  return make<rec_expr>(t, e);
}

} // namespace sys_int
} // namespace beaker