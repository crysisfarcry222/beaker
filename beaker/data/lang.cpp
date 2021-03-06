// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "lang.hpp"
#include <beaker/data/construction/builder.hpp>
#include <beaker/data/comparison/eq.hpp>
#include <beaker/data/comparison/hash.hpp>
#include <beaker/data/printing/print.hpp>
#include <beaker/data/generation/gen.hpp>

#include <cassert>


namespace beaker {
namespace data {

/// Allocate a builder for the language set.
static void* make_builder(module& m) { return new builder(m); }

feature::feature()
  : basic_feature<data_lang>(make_builder)
{
  add_algorithm<eq_algo>();
  add_algorithm<hash_algo>();
  add_algorithm<print_algo>();
  add_algorithm<gen_algo>();
}

} // namespace data
} // namespace beaker
