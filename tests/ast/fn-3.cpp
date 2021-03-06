// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include <beaker/util/memory.hpp>
#include <beaker/util/symbol_table.hpp>

#include <beaker/base/module.hpp>
#include <beaker/base/generation/generation.hpp>
#include <beaker/base/printing/print.hpp>

#include <beaker/logic/lang.hpp>
#include <beaker/logic/type.hpp>
#include <beaker/logic/expr.hpp>
#include <beaker/logic/decl.hpp>
#include <beaker/logic/construction/builder.hpp>

#include <beaker/numeric/lang.hpp>
#include <beaker/numeric/type.hpp>
#include <beaker/numeric/expr.hpp>
#include <beaker/numeric/construction/builder.hpp>

#include <beaker/core/lang.hpp>
#include <beaker/core/name.hpp>
#include <beaker/core/type.hpp>
#include <beaker/core/expr.hpp>
#include <beaker/core/decl.hpp>
#include <beaker/core/stmt.hpp>
#include <beaker/core/construction/builder.hpp>

#include <llvm/IR/Type.h>
#include <llvm/IR/Module.h>

#include <iostream>


using namespace beaker;

int
main(int argc, char* argv[])
{
  language lang;
  lang.add_feature<logic::feature>();
  lang.add_feature<numeric::feature>();
  lang.add_feature<core::feature>();

  module mod;
  logic::builder& lb = mod.get_builder<logic::builder>();
  numeric::builder& nb = mod.get_builder<numeric::builder>();
  core::builder& cb = mod.get_builder<core::builder>();

  // Object types.
  type& v = cb.get_void_type();
  type& b = lb.get_bool_type();
  type& i32 = nb.get_int_type(32);
  type& i1024 = nb.get_int_type(1024);

  // Value types.
  type& rb = cb.get_ref_type(b);
  type& ri32 = cb.get_ref_type(i32);
  type& ri1024 = cb.get_ref_type(i1024);

  //Values
  expr& t = lb.get_true_expr();
  expr& z1 = nb.make_int_expr(i32, 42);
  expr& z2 = nb.make_int_expr(i1024, 5);

  // Passes a couple objects by reference, returns one by copy.
  typed_decl* d1;
  { // f1 : (bool&, int32&) -> i32
    decl* vars[] {
      &cb.make_parm_decl("a", rb),
      &cb.make_parm_decl("b", ri32),
      &cb.make_parm_decl("r", i32)
    };

    decl_seq parms {vars[0], vars[1]};
    decl& ret = *vars[2];
    type& type = cb.get_fn_type(parms, ret);

    expr& ref = cb.make_ref_expr(parms[1]); // ref a
    expr& val = cb.make_deref_expr(ref); // deref ref a
    expr& def = cb.make_copy_init(val); // = copy deref ref a
    
    d1 = &cb.make_fn_decl(cb.get_name("f1"), type, parms, ret, def);
    mod.add_declaration(*d1);
  }

  // Passes a couple objects by reference, returns one by copy.
  typed_decl* d2;
  { // f1 : (int1024&, int1024) -> void
    decl_seq vars {
      &cb.make_parm_decl("dst", ri1024),
      &cb.make_parm_decl("src", i1024),
      &cb.make_parm_decl("ret", v)
    };
    decl_seq parms {&vars[0], &vars[1]};
    decl& ret = vars[2];
    type& type = cb.get_fn_type(parms, ret);

    expr& def = cb.make_void_expr( // ref dst = deref ref src
      cb.make_assign_expr(
        cb.make_ref_expr(vars[0]), 
        cb.make_deref_expr(cb.make_ref_expr(vars[1]))
      )
    );

    d2 = &cb.make_fn_decl("f2", type, parms, ret, def);
    mod.add_declaration(*d2);
  }

  // Build a main function that declares some variables to be passed
  // by reference.

  // Make some variables and their initializers.
  decl_seq vars {
    &cb.make_var_decl("b", b, t),
    &cb.make_var_decl("z1", i32, z1),
    &cb.make_var_decl("z2", i1024, z2),
    &cb.make_var_decl("z3", i1024, cb.make_nop_init(i1024)),
  };

  // Generate a call expression to f1.
  expr& f1 = cb.make_ref_expr(*d1);
  expr_seq args1 {
    &cb.make_ref_init(cb.make_ref_expr(vars[0])),
    &cb.make_ref_init(cb.make_ref_expr(vars[1]))
  };
  expr& call1 = cb.make_call_expr(f1, args1);
  decl& assert1 = lb.make_assert_decl(nb.make_eq_expr(call1, z1));

  // Generate a call expression to f2.
  expr& f2 = cb.make_ref_expr(*d2);
  expr_seq args2 {
    &cb.make_ref_init(cb.make_ref_expr(vars[3])),
    &cb.make_copy_init(cb.make_deref_expr(cb.make_ref_expr(vars[2])))
  };
  expr& call2 = cb.make_call_expr(f2, args2);
  decl& assert2 = lb.make_assert_decl( // assert deref ref z3 == deref ref z2
    nb.make_eq_expr(
      cb.make_deref_expr(cb.make_ref_expr(vars[3])),
      cb.make_deref_expr(cb.make_ref_expr(vars[2]))
    )
  );

  stmt_seq stmts {
    &cb.make_decl_stmt(vars[0]), // var bool b = true;
    &cb.make_decl_stmt(vars[1]), // var int32 z1 = 42
    &cb.make_decl_stmt(vars[2]), // var int1024 z2 = 5
    &cb.make_decl_stmt(vars[3]), // var int1024 z3 = nop;
    // &cb.make_expr_stmt(call1),   // f1(b, z1) -- generated by the assertion
    &cb.make_decl_stmt(assert1), // assert f1(b, z1) == z1
    &cb.make_expr_stmt(call2),   // f2(z3, z2)
    &cb.make_decl_stmt(assert2), // f2(z3, z2)
    &cb.make_ret_stmt(z1)        // return z1
  };
  stmt& def = cb.make_block_stmt(stmts);

  // Build a wrapper function.
  decl_seq parms;
  decl& ret = cb.make_parm_decl("ret", i32);
  type& ftype = cb.get_fn_type(parms, ret);
  decl& fn = cb.make_fn_decl("main", ftype, parms, ret, def);
  mod.add_declaration(fn);

  print(mod);

  // Emit LLVM.
  generator gen("out.ll");
  generate(gen, mod);
  gen.get_module().dump();
}
