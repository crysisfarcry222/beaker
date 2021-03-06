// Copyright (c) 2015-2017 Andrew Sutton
// All rights reserved

#ifndef BEAKER_SYS_FN_SERIALIZATION_WRITE_HPP
#define BEAKER_SYS_FN_SERIALIZATION_WRITE_HPP

#include <beaker/base/serialization/writer.hpp>


namespace beaker {
namespace sys_fn {

struct write_algo : write_algorithm
{
  void operator()(archive_writer&, const type&) const override;
  void operator()(archive_writer&, const expr&) const override;
  void operator()(archive_writer&, const decl&) const override;
  void operator()(archive_writer&, const stmt&) const override;
};

} // namespace sys_fn
} // namespace beaker


#endif
