// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_SYS_FLOAT_EXPRESSION_HPP
#define BEAKER_SYS_FLOAT_EXPRESSION_HPP

#include <beaker/base/expr.hpp>


namespace beaker {
namespace sys_float {

enum {
  first_expr_kind = sys_float_lang_block,
#define def_expr(e) e ## _expr_kind,
#include "expr.def"
  last_expr_kind
};


// -------------------------------------------------------------------------- //
// Expressions

/// Represents floating point literals.
///
/// The type of the expression shall be a floating point type.
struct float_expr : literal_expr_impl<float_expr_kind>
{
  using literal_expr_impl<float_expr_kind>::literal_expr_impl;
};


/// Represents the expression `e1 == e2`.
///
/// The operands `e1` and `e2` shall have the same type `t`, and `t` shall be 
/// an arithmetic type. The type of the expression shall be `bool`.
///
/// The value of the expression is `true` when the values of `e1` and `e2` 
/// are equal.
struct eq_expr : binary_expr_impl<eq_expr_kind> 
{
  using binary_expr_impl<eq_expr_kind>::binary_expr_impl;
};


/// Represents the expression `e1 != e2`.
///
/// The operands `e1` and `e2` shall have the same type `t`, and `t` shall be 
/// an arithmetic type. The type of the expression shall be `bool`.
///
/// The value of the expression is `true` when the values of `e1` and `e2` 
/// are different.
struct ne_expr : binary_expr_impl<ne_expr_kind> 
{
  using binary_expr_impl<ne_expr_kind>::binary_expr_impl;
};


/// Represents the expression `e1 < e2`.
///
/// The operands `e1` and `e2` shall have the same type `t`, and `t` shall be 
/// an arithmetic type. The type of the expression shall be `bool`.
///
/// The value of the expression is `true` when the value of `e1` is less than
/// the value of `e2`.
struct lt_expr : binary_expr_impl<lt_expr_kind> 
{
  using binary_expr_impl<lt_expr_kind>::binary_expr_impl;
};


/// Represents the expression `e1 > e2`.
///
/// The operands `e1` and `e2` shall have the same type `t`, and `t` shall be 
/// an arithmetic type. The type of the expression shall be `bool`.
///
/// The value of the expression is `true` when the value of `e1` is greater 
/// than the value of `e2`.
struct gt_expr : binary_expr_impl<gt_expr_kind> 
{
  using binary_expr_impl<gt_expr_kind>::binary_expr_impl;
};


/// Represents the expression `e1 <= e2`.
///
/// The operands `e1` and `e2` shall have the same type `t`, and `t` shall be 
/// an arithmetic type. The type of the expression shall be `bool`.
///
/// The value of the expression is `true` when the value of `e1` is less than
/// or equal to the value of `e2`.
struct le_expr : binary_expr_impl<le_expr_kind> 
{
  using binary_expr_impl<le_expr_kind>::binary_expr_impl;
};


/// Represents the expression `e1 >= e2`.
///
/// The operands `e1` and `e2` shall have the same type `t`, and `t` shall be 
/// an arithmetic type. The type of the expression shall be `bool`.
///
/// The value of the expression is `true` when the value of `e1` is greater
/// than or equal to the value of `e2`.
struct ge_expr : binary_expr_impl<ge_expr_kind> 
{
  using binary_expr_impl<ge_expr_kind>::binary_expr_impl;
};


/// Represents the expression `e1 + e2`.
///
/// The operands `e1` and `e2` shall have the same type `t`, and `t` shall be 
/// an arithmetic type. The type of the expression shall also be `t`.
///
/// The value of the expression is the sum of the values of `e1` and `e2`.
struct add_expr : binary_expr_impl<add_expr_kind> 
{
  using binary_expr_impl<add_expr_kind>::binary_expr_impl;
};


/// Represents the expression `e1 - e2`.
///
/// \todo Document me.
struct sub_expr : binary_expr_impl<sub_expr_kind> 
{
  using binary_expr_impl<sub_expr_kind>::binary_expr_impl;
};


/// Represents the expression `e1 * e2`.
///
/// \todo Document me.
struct mul_expr : binary_expr_impl<mul_expr_kind> 
{
  using binary_expr_impl<mul_expr_kind>::binary_expr_impl;
};


/// Represents the expression `e1 / e2`.
///
/// \todo Document me.
struct div_expr : binary_expr_impl<div_expr_kind> 
{
  using binary_expr_impl<div_expr_kind>::binary_expr_impl;
};


/// Represents the negation expression `-e`.
///
/// \todo Document me.
struct neg_expr : unary_expr_impl<neg_expr_kind> 
{
  using unary_expr_impl<neg_expr_kind>::unary_expr_impl;
};


/// Represents the reciprocal expression `/e`.
///
/// \todo Document me.
struct rec_expr : unary_expr_impl<rec_expr_kind> 
{
  using unary_expr_impl<rec_expr_kind>::unary_expr_impl;
};


// -------------------------------------------------------------------------- //
// Operations

bool is_floating_point_expression(const expr&);

} // namespace sys_float
} // namespace beaker


#endif
