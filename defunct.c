#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <editline/readline.h>
#include <editline/history.h>


struct terms {
  mpc_parser_t* number;
  mpc_parser_t* mathOperator;
  mpc_parser_t* sExpression;
  mpc_parser_t* program;
};

enum { LVAL_NUMBER, LVAL_ERROR };

enum { LERR_DIVISON_BY_ZERO, LERR_UNKNOWN_OPERATOR, LERR_BAD_NUMBER };

typedef struct {
  int type;
  long double number;
  int error;
} lval;


lval number_to_lval(long double number) {
  lval val;
  val.type = LVAL_NUMBER;
  val.number = number;
  return val;
}

lval error_to_lval(int errorIndex) {
  lval val;
  val.type = LVAL_ERROR;
  val.error = errorIndex;
  return val;
}


struct terms defineGrammar() {
  struct terms t = {
    .number = mpc_new("number"),
    .mathOperator = mpc_new("mathOperator"),
    .sExpression = mpc_new("sexpression"),
    .program = mpc_new("program")
  };

  mpca_lang(MPCA_LANG_DEFAULT,
  "\
    number       :  /-?[0-9]+/ ;                                       \
    mathOperator :  '+' | '-' | '*' | '/' ;                            \
    sexpression  :  <number> | '[' <mathOperator> <sexpression>+ ']' ; \
    program      :  /^/ <mathOperator> <sexpression>+ /$/ ;            \
  ",
   t.number, t.mathOperator, t.sExpression, t.program);

  return t;
}

void teardown(struct terms t) {
  mpc_cleanup(4, t.number, t.mathOperator, t.sExpression, t.program);
}

lval sum(long double x, long double y) {
  return number_to_lval(x + y);
}

lval subtract(long double x, long double y) {
  return number_to_lval(x - y);
}

lval multiply(long double x, long double y) {
  return number_to_lval(x * y);
}

lval divide(long double x, long double y) {
  if(y == 0) {
    return error_to_lval(LERR_DIVISON_BY_ZERO);
  }
  return number_to_lval(x / y);
}

lval eval_operator(lval x, char* operator, lval y) {
  if(x.type == LVAL_ERROR) {
    return x;
  }
  if(y.type == LVAL_ERROR) {
    return y;
  }

  if      (strcmp(operator, "+") == 0) { return sum(x.number,y.number); }
  else if (strcmp(operator, "-") == 0) { return subtract(x.number,y.number); }
  else if (strcmp(operator, "*") == 0) { return multiply(x.number,y.number); }
  else if (strcmp(operator, "/") == 0) { return divide(x.number,y.number); }

  else return error_to_lval(LERR_UNKNOWN_OPERATOR);
}


lval eval(mpc_ast_t* node) {

  if (strstr(node->tag, "number")) {
    errno = 0;
    long double x = strtod(node->contents, NULL);
    return errno != ERANGE ? number_to_lval(x) : error_to_lval(LERR_BAD_NUMBER);
  }
  

  int i = 1;
  char* operator = node->children[i++]->contents;
  lval expression = eval(node->children[i++]);
  
  while (strstr(node->children[i]->tag, "sexpression")) {
    expression = eval_operator(expression, operator, eval(node->children[i]));
    i++;
  }
  
  return expression; 
}

void print_lval(lval lval) {
  switch (lval.type) {
    case LVAL_NUMBER: printf("%Lf", lval.number); break;

    case LVAL_ERROR:
      if (lval.error == LERR_DIVISON_BY_ZERO) { printf("Error: division by zero."); }
      if (lval.error == LERR_UNKNOWN_OPERATOR)   { printf("Error: unknown operator."); }
      if (lval.error == LERR_BAD_NUMBER)         { printf("Error: bad number"); }
    break;
  }
  printf("\n");
}


void print_result(mpc_result_t abstractSyntaxtTree) {
  lval result = eval(abstractSyntaxtTree.output);
  print_lval(result);
  mpc_ast_delete(abstractSyntaxtTree.output);
}

void print_error(mpc_result_t result) {
  mpc_err_print(result.error);
  mpc_err_delete(result.error);
}




int main(int argc, char** argv) {

  puts(">> Defunct v0.3");

  struct terms terms = defineGrammar();

  while (1) {
    char* input = readline(": ");

    mpc_result_t abstractSyntaxtTree;
    if (mpc_parse("<stdin>", input, terms.program, &abstractSyntaxtTree)) {
      print_result(abstractSyntaxtTree);
    } else {
      print_error(abstractSyntaxtTree);
    }
  }

  teardown(terms);

  return 0;
}
