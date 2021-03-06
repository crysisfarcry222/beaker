// Copyright (c) 2015-2017 Andrew Sutton
// All rights reserved

#ifndef BEAKER_EVALUATION_EVALUATE_HPP
#define BEAKER_EVALUATION_EVALUATE_HPP

#include <beaker/base/lang.hpp>
#include <beaker/base/error.hpp>
#include <beaker/base/value.hpp>


namespace beaker {

/// Represents an error that occurred during evaluation. These exceptions can
/// be caught and suppressed to support compile-time evaluation.
///
/// \todo: Allow errors to indicate declarations and statements also.
struct evaluation_error : error
{
  using error::error;
};


/// The evaluator maintains the evaluation context for the evaluation of the
/// expressions, declarations, and statements.
///
/// The evaluator maintains a reference to the language. This allows evaluation
/// to be defined in terms of the parameterization of the language by 
/// translation and compilation options.
struct evaluator
{
  evaluator(const language&);
  
  const language& lang;
};

inline evaluator::evaluator(const language& lang) : lang(lang) { }


value evaluate(evaluator&, const expr&);
value evaluate(evaluator&, const decl&);
value evaluate(evaluator&, const stmt&);


// -------------------------------------------------------------------------- //
// Dispatch interface

/// Evaluate the given expression. Behavior is undefined if overload resolution
/// selects this function.
inline value 
evaluate_expr(evaluator&, const expr&)
{
  assert(false && "function not defined");
}

/// Elaborate the given declaration. Behavior is undefined if overload 
/// resolution selects this function.
inline value 
evaluate_decl(evaluator&, const decl&)
{
  assert(false && "function not defined");
}

/// Evaluate the given statement. Behavior is undefined if overload resolution
/// selects this function.
inline value 
evaluate_stmt(evaluator&, const stmt&)
{
  assert(false && "function not defined");
}


} // namespace beaker


#endif
