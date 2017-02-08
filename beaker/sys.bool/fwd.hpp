// Copyright (c) 2015-2017 Andrew Sutton
// All rights reserved

#ifndef BEAKER_SYS_BOOL_FWD_HPP
#define BEAKER_SYS_BOOL_FWD_HPP


namespace beaker {
namespace sys_bool {

struct feature;
#define def_type(T, B) struct T ## _type;
#define def_expr(E, B) struct E ## _expr;
#include "type.def"
#include "expr.def"
struct builder;

} // namespace sys_bool
} // namespace beaker


#endif
