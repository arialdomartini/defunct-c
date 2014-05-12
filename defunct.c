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

long double sum(long double x, long double y) {
  return x + y;
}

long double subtract(long double x, long double y) {
  return x - y;
}

long double multiply(long double x, long double y) {
  return x * y;
}

long double divide(long double x, long double y) {
  return x / y;
}

long double eval_operator(long double x, char* operator, long double y) {
  if (strcmp(operator, "+") == 0) { return sum(x,y); }
  else if (strcmp(operator, "-") == 0) { return subtract(x,y); }
  else if (strcmp(operator, "*") == 0) { return multiply(x,y); }
  else if (strcmp(operator, "/") == 0) { return divide(x,y); }
  else exit(-1);
}

long double eval(mpc_ast_t* node) {
  
  if (strstr(node->tag, "number")) { 
    return atoi(node->contents); 
  }

  int i = 1;
  char* operator = node->children[i++]->contents;
  long double expression = eval(node->children[i++]);
  
  while (strstr(node->children[i]->tag, "sexpression")) {
    expression = eval_operator(expression, operator, eval(node->children[i]));
    i++;
  }
  
  return expression; 
}


void print_result(mpc_result_t abstractSyntaxtTree) {
  long double result = eval(abstractSyntaxtTree.output);
  printf("%Lf\n", result);
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