// Copyright (c) 2015-2017 Andrew Sutton
// All rights reserved

#include "eq.hpp"
#include "../type.hpp"
#include "../expr.hpp"


namespace beaker {
namespace sys_float {


/// Returns true when numeric types `a` and `b` have the same precision.
bool
eq_algo::operator()(const type& a, const type& b) const
{
  switch (a.get_kind()) {
#define def_type(T) \
    case T##_type_kind: \
      return cast<T##_type>(a).get_precision() == cast<T##_type>(b).get_precision();
#include "../type.def"
    default:
      break;
  }
  assert(false && "not a floating point type");
}

/// \todo Implement me.
bool
eq_algo::operator()(const expr& a, const expr& b) const
{
  assert(false && "not implemented");
}

} // namespace sys_float
} // namespace beaker
