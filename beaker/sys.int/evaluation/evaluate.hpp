// Copyright (c) 2015-2017 Andrew Sutton
// All rights reserved

#ifndef BEAKER_SYS_INT_EVALUATION_EVALUATE_HPP
#define BEAKER_SYS_INT_EVALUATION_EVALUATE_HPP

#include <beaker/sys.int/fwd.hpp>

#include <beaker/base/evaluation/evaluate.hpp>


namespace beaker {
namespace sys_int {

value evaluate_int_expr(evaluator&, const int_expr&);
value evaluate_eq_expr(evaluator&, const eq_expr&);
value evaluate_ne_expr(evaluator&, const ne_expr&);
value evaluate_lt_expr(evaluator&, const lt_expr&);
value evaluate_gt_expr(evaluator&, const gt_expr&);
value evaluate_le_expr(evaluator&, const le_expr&);
value evaluate_ge_expr(evaluator&, const ge_expr&);
value evaluate_add_expr(evaluator&, const add_expr&);
value evaluate_sub_expr(evaluator&, const sub_expr&);
value evaluate_mul_expr(evaluator&, const mul_expr&);
value evaluate_div_expr(evaluator&, const div_expr&);
value evaluate_rem_expr(evaluator&, const rem_expr&);
value evaluate_neg_expr(evaluator&, const neg_expr&);
value evaluate_rec_expr(evaluator&, const rec_expr&);

} // namespace sys_int
} // namespace beaker


#endif