#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include ".auto/all_nodes.h" // all_nodes.h is automatically generated

//---------------------------------------------------------------------------

void l22::postfix_writer::do_nil_node(cdk::nil_node *const node, int lvl)
{
  // EMPTY
}
void l22::postfix_writer::do_data_node(cdk::data_node *const node, int lvl)
{
  // EMPTY
}
void l22::postfix_writer::do_double_node(cdk::double_node *const node, int lvl)
{
  // EMPTY
}
void l22::postfix_writer::do_not_node(cdk::not_node *const node, int lvl)
{
  // EMPTY
}
void l22::postfix_writer::do_and_node(cdk::and_node *const node, int lvl)
{
  // EMPTY
}
void l22::postfix_writer::do_or_node(cdk::or_node *const node, int lvl)
{
  // EMPTY
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_sequence_node(cdk::sequence_node *const node, int lvl)
{
  for (size_t i = 0; i < node->size(); i++)
  {
    node->node(i)->accept(this, lvl);
  }
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_integer_node(cdk::integer_node *const node, int lvl)
{
  _pf.INT(node->value()); // push an integer
}

void l22::postfix_writer::do_string_node(cdk::string_node *const node, int lvl)
{
  int lbl1;

  /* generate the string */
  _pf.RODATA();                    // strings are DATA readonly
  _pf.ALIGN();                     // make sure we are aligned
  _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
  _pf.SSTRING(node->value());      // output string characters

  /* leave the address on the stack */
  _pf.TEXT();            // return to the TEXT segment
  _pf.ADDR(mklbl(lbl1)); // the string to be printed
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_neg_node(cdk::neg_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  _pf.NEG();                           // 2-complement
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_add_node(cdk::add_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.ADD();
}
void l22::postfix_writer::do_sub_node(cdk::sub_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.SUB();
}
void l22::postfix_writer::do_mul_node(cdk::mul_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MUL();
}
void l22::postfix_writer::do_div_node(cdk::div_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.DIV();
}
void l22::postfix_writer::do_mod_node(cdk::mod_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}
void l22::postfix_writer::do_lt_node(cdk::lt_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.LT();
}
void l22::postfix_writer::do_le_node(cdk::le_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.LE();
}
void l22::postfix_writer::do_ge_node(cdk::ge_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.GE();
}
void l22::postfix_writer::do_gt_node(cdk::gt_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.GT();
}
void l22::postfix_writer::do_ne_node(cdk::ne_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.NE();
}
void l22::postfix_writer::do_eq_node(cdk::eq_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.EQ();
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_variable_node(cdk::variable_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  // simplified generation: all variables are global
  _pf.ADDR(node->name());
}

void l22::postfix_writer::do_rvalue_node(cdk::rvalue_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
  _pf.LDINT(); // depends on type size
}

void l22::postfix_writer::do_assignment_node(cdk::assignment_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->rvalue()->accept(this, lvl); // determine the new value
  _pf.DUP32();
  if (new_symbol() == nullptr)
  {
    node->lvalue()->accept(this, lvl); // where to store the value
  }
  else
  {
    _pf.DATA();                      // variables are all global and live in DATA
    _pf.ALIGN();                     // make sure we are aligned
    _pf.LABEL(new_symbol()->name()); // name variable location
    reset_new_symbol();
    _pf.SINT(0);                       // initialize it to 0 (zero)
    _pf.TEXT();                        // return to the TEXT segment
    node->lvalue()->accept(this, lvl); // DAVID: bah!
  }
  _pf.STINT(); // store the value at address
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_program_node(l22::program_node *const node, int lvl)
{
  // Note that Simple doesn't have functions. Thus, it doesn't need
  // a function node. However, it must start in the main function.
  // The ProgramNode (representing the whole program) doubles as a
  // main function node.

  // generate the main function (RTS mandates that its name be "_main")
  _pf.TEXT();
  _pf.ALIGN();
  _pf.GLOBAL("_main", _pf.FUNC());
  _pf.LABEL("_main");
  _pf.ENTER(0); // Simple doesn't implement local variables

  node->statements()->accept(this, lvl);

  // end the main function
  _pf.INT(0);
  _pf.STFVAL32();
  _pf.LEAVE();
  _pf.RET();

  // these are just a few library function imports
  _pf.EXTERN("readi");
  _pf.EXTERN("printi");
  _pf.EXTERN("prints");
  _pf.EXTERN("println");
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_evaluation_node(l22::evaluation_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  if (node->argument()->is_typed(cdk::TYPE_INT))
  {
    _pf.TRASH(4); // delete the evaluated value
  }
  else if (node->argument()->is_typed(cdk::TYPE_STRING))
  {
    _pf.TRASH(4); // delete the evaluated value's address
  }
  else
  {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
}

void l22::postfix_writer::do_print_node(l22::print_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  for (size_t ix = 0; ix < node->arguments()->size(); ix++)
  {
    auto child = dynamic_cast<cdk::expression_node *>(node->arguments()->node(ix));
    std::shared_ptr<cdk::basic_type> etype = child->type();
    child->accept(this, lvl); // expression to print
    if (etype->name() == cdk::TYPE_INT)
    {
      _pf.CALL("printi");
      _pf.TRASH(4); // trash int
    }
    else if (etype->name() == cdk::TYPE_STRING)
    {
      _pf.CALL("prints");
      _pf.TRASH(4); // trash char pointer
    }
    else
    {
      std::cerr << "cannot print expression of unknown type" << std::endl;
      return;
    }
  }

  if (node->newline())
  {
    _pf.CALL("println");
  }
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_while_node(l22::while_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  _pf.LABEL(mklbl(lbl1 = ++_lbl));
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl2 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl1));
  _pf.LABEL(mklbl(lbl2));
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_if_node(l22::if_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_if_else_node(l22::if_else_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.LABEL(mklbl(lbl1));
  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1 = lbl2));
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_again_node(l22::again_node *node, int lvl)
{
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_block_node(l22::block_node *node, int lvl)
{
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_function_call_node(l22::function_call_node *node, int lvl)
{
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_lambda_node(l22::lambda_node *node, int lvl)
{
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_return_node(l22::return_node *node, int lvl)
{
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_stop_node(l22::stop_node *node, int lvl)
{
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_declaration_node(l22::declaration_node *node, int lvl)
{
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_nullptr_node(l22::nullptr_node *node, int lvl)
{
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_address_of_node(l22::address_of_node *node, int lvl)
{
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_index_node(l22::index_node *node, int lvl)
{
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_input_node(l22::input_node *node, int lvl)
{
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_sizeof_node(l22::sizeof_node *node, int lvl)
{
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_stack_alloc_node(l22::stack_alloc_node *node, int lvl)
{
}