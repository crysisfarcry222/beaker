// Copyright (c) 2015-2017 Andrew Sutton
// All rights reserved

#include "decl.hpp"
#include "type.hpp"


namespace beaker {
namespace sys_fn {

/// Returns the type of the function.
const fn_type& fn_decl::get_type() const
{ 
  return cast<fn_type>(mapping_decl::get_type()); 
}

/// Returns the type of the function.
fn_type& fn_decl::get_type()
{ 
  return cast<fn_type>(mapping_decl::get_type()); 
}

} // namespace sys_fn
} // namespace beaker

