// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "generation.hpp"

#include <beaker/base/name.hpp>
#include <beaker/base/type.hpp>
#include <beaker/base/expr.hpp>
#include <beaker/base/decl.hpp>
#include <beaker/base/stmt.hpp>
#include <beaker/base/module.hpp>
#include <beaker/util/symbol.hpp>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/raw_os_ostream.h>

#include <algorithm>
#include <iostream>
#include <beaker/base/printing/print.hpp>


namespace beaker {

generator::generator(const char* n)
  : parent_(nullptr),
    cxt_(new llvm::Context()), 
    mod_(new llvm::Module(n, *cxt_)),
    fn_(),
    entry_(),
    exit_(),
    block_(),
    top_(),
    bottom_(),
    init_()
{
  enter_decl_context();
}

// Initialize the generator so that it shares the LLVM context, module,
// and global name binding environment as gen.
generator::generator(generator& gen)
  : parent_(&gen),
    cxt_(gen.cxt_),
    mod_(gen.mod_),
    fn_(),
    entry_(),
    block_(),
    top_(),
    bottom_(),
    init_()
{
  // Push all global declarations into the current declaration context.
  //
  // TODO: This isn't particularly efficient. It would be nice if we
  // could actually link declaration contexts so there was a single
  // global context.
  enter_decl_context();
  std::vector<const decl*> decls = gen.decl_stack_[0];
  for (const decl* d : decls)
    put_value(*d, gen.get_value(*d));
}

generator::~generator()
{
  leave_decl_context();

  if (parent_) {
    // Merge any notes into the parent module before going away. 
    parent_->notes_.insert(notes_.end(), notes_.begin(), notes_.end());
  }
  else {
    // Only delete the context if this is the root generator. Otherwise, this
    // would delete the module.
    delete cxt_;

    // Now, we can delete the notes.
    for (cg::note* p : notes_)
      delete p;
  }
}

/// Returns the function information associated with t.
const cg::fn_info& 
generator::get_function_info(const type& t)
{
  return fn_info_.find(&t)->second;
}

/// Associate the function information with t.
void 
generator::put_function_info(const type& t, const cg::fn_info& fi)
{
  fn_info_.emplace(&t, fi);
}

/// Associate the function information with t.
void 
generator::put_function_info(const type& t, cg::fn_info&& fi)
{
  fn_info_.emplace(&t, fi);
}

// Enter a new declaration context.
//
// This simply pushes a new value list onto the value stack.
void
generator::enter_decl_context()
{
  decl_stack_.emplace_back();
}

// Leave a declaration context.
//
// Remove all saved declarations in the current context.
void
generator::leave_decl_context()
{
  std::vector<const decl*>& decls = decl_stack_.back();
  std::for_each(decls.begin(), decls.end(), [&](const decl* d) {
    decl_env_.erase(d);
  });
  decl_stack_.pop_back();
}

// Associate the given declaration with it's generated LLVM value.
//
// A declaration may be registered multiple times, but it must be
// registered to the same value in each case.
void
generator::put_value(const decl& d, llvm::Value* v)
{
  std::vector<const decl*>& decls = decl_stack_.back();
  auto result = decl_env_.emplace(&d, v);
  if (result.second) {
    assert(result.first->second == v);
    decls.push_back(&d);
  }
}

/// Returns the value associated with the declaration. The declaration must have 
/// been previously bound to a value.
///
/// \todo Support out-of-order code generation. We should be able to step
/// outside the current definition and generate a new declaration as needed,
/// at least for global declarations.
llvm::Value*
generator::get_value(const decl& d)
{
  auto iter = decl_env_.find(&d);
  assert(iter != decl_env_.end());
  return iter->second;

#if 0
  if (iter == decl_env_.end()) {
    // If we haven't previously seen the declaration, step outside the current
    // generation, generate the referenced declaration, and then return that
    // value.
    //
    // FIXME: This doesn't actually work. Rewrite the generator so that it's
    // internal state (current function, loop info, etc.) can be stacked
    // using RAII guards.
    generator gen(*this);
    cg::value val = generate(gen, d);
    return val;
  }
#endif
}

/// Returns true if we've already seen the declaratoin.
bool
generator::seen_decl(const decl& d)
{
  return decl_env_.find(&d) != decl_env_.end();
}

/// Define f to be a function by creating an entry block. This makes f
/// the current function and the entry block the current basic block.
void
generator::define_function(llvm::Function* f) 
{
  assert(!fn_);
  enter_decl_context();
  fn_ = f;
  // Create the entry block as the first block of the function.
  entry_ = llvm::BasicBlock::Create(*cxt_, "entry", fn_);
  
  // Create the exit block, but don't attach it to the function. We'll do
  // that when the function is complete.
  exit_ = llvm::BasicBlock::Create(*cxt_, "exit");
  block_ = entry_;
}

/// Exit the scope of the current function and reset all info relative
/// to the current function.
void
generator::end_function()
{
  assert(fn_);

  // Link the exit block into the function.
  exit_->insertInto(fn_);

  fn_ = nullptr;
  entry_ = nullptr;
  exit_ = nullptr;
  block_ = nullptr;
  leave_decl_context();
}

/// Sets the current instruction block to b. If the block is unattached, then
/// add append it to the current block list.
void 
generator::set_current_block(llvm::BasicBlock* b)
{
  if (!b->getParent())
    b->insertInto(fn_);
  block_ = b; 
}


/// Create a local object for the type `t`. Local variables are always inserted
/// into the front of the entry block.
cg::value
generator::make_alloca(cg::type t, const char* n)
{
  llvm::BasicBlock* bb = get_entry_block();
  llvm::Builder ir(bb, bb->begin());
  return ir.CreateAlloca(t, nullptr, n);
}

cg::value
generator::make_alloca(cg::type t, const std::string& n)
{
  return make_alloca(t, n.c_str());
}

/// Print the text of the module to standard output.
void
generator::print()
{
  mod_->print(llvm::outs(), nullptr);
}

/// Print the text of the module to the given output stream.
void
generator::print(std::ostream& os)
{
  llvm::raw_os_ostream out(os);
  mod_->print(out, nullptr);
}

// -------------------------------------------------------------------------- //
// Dispatch infrastructure

/// The default behavior is not defined.
std::string
generate_algorithm::operator()(generator&, const name&) const
{
  assert(false && "not defined");
}

/// The default behavior is not defined.
cg::type
generate_algorithm::operator()(generator&, const type&) const
{
  assert(false && "not defined");
}

/// The default behavior is not defined.
cg::value
generate_algorithm::operator()(generator&, const expr&) const
{
  assert(false && "not defined");
}

/// The default behavior is not defined.
cg::value
generate_algorithm::operator()(generator&, const decl&) const
{
  assert(false && "not defined");
}

/// The default behavior is not defined.
void
generate_algorithm::operator()(generator&, const stmt&) const
{
  assert(false && "not defined");
}

/// Generate code for the declarations in m.
void
generate(generator& gen, const module& m)
{
  gen.enter_source_module(m);
  for (const decl& d : m.get_declarations())
    generate(gen, d);
  gen.leave_source_module();
}

// Returns the codegen algorithm associated with the node t.
template<typename T>
static inline const generate_algorithm&
get_generate(const T& t)
{
  feature& feat = language::get_feature(t);
  return feat.template get_algorithm<generate_algorithm>();
}

/// Generate a symbol name from n.
///
/// TODO: The formation of the name is determined by the ABI. In order to be
/// C++ compliant, for example, we would need to mangle as per the Itanium
/// ABI specification (among many other things).
std::string
generate(generator& gen, const name& n)
{
  return get_generate(n)(gen, n);
}

// Generate an LLVM type from t.
static cg::type
do_generate_type(generator& gen, const type& t)
{
  return get_generate(t)(gen, t);
}

/// Generate the LLVM representation of t.
///
/// If the type has previously been seen, return its cached version.
///
/// TODO: Verify that t is a canonical type.
cg::type
generate(generator& gen, const type& t)
{
  type_map& types = gen.get_types();
  auto iter = types.find(&t);
  if (iter != types.end())
    return iter->second;
  cg::type ret = do_generate_type(gen, t);
  types.emplace(&t, ret);
  return ret;
}

/// Generate a sequence of LLVM instructions for e.
cg::value
generate(generator& gen, const expr& e)
{
  return get_generate(e)(gen, e);
}

/// Generate an LLVM declaration for d. 
cg::value
generate(generator& gen, const decl& d)
{
  // FIXME: Don't generate the declaration more than once.
  return get_generate(d)(gen, d);
}

/// Generate a sequence of blocks and instructions for s.
void
generate(generator& gen, const stmt& s)
{
  return get_generate(s)(gen, s);
}

} // namespace beaker
