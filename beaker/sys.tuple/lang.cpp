// Copyright (c) 2015-2017 Andrew Sutton
// All rights reserved

#include "lang.hpp"
#include "build.hpp"
#include "comparison/eq.hpp"
#include "comparison/hash.hpp"
#include "printing/print.hpp"
#include "generation/gen.hpp"
#include "serialization/write.hpp"

namespace beaker {
namespace sys_tuple {

/// Allocates a builder.
static void* make_builder(module& m) { return new builder(m); }

feature::feature(language& lang)
  : feature_impl<sys_tuple_lang>(lang, make_builder)
{
  add_algorithm<eq_algo>();
  add_algorithm<hash_algo>();
  add_algorithm<print_algo>();
  add_algorithm<gen_algo>();
  add_algorithm<write_algo>();
}

} // namespace sys_tuple
} // namespace beaker
