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



long eval_operator(long x, char* operator, long y) {
  if (strcmp(operator, "+") == 0) { return x + y; }
  if (strcmp(operator, "-") == 0) { return x - y; }
  if (strcmp(operator, "*") == 0) { return x * y; }
  if (strcmp(operator, "/") == 0) { return x / y; }
  return 0;
}

long eval(mpc_ast_t* node) {
  
  if (strstr(node->tag, "number")) { 
    return atoi(node->contents); 
  }

  int i = 1;
  char* operator = node->children[i++]->contents;
  long thirdChild = eval(node->children[i++]);
  
  while (strstr(node->children[i]->tag, "sexpression")) {
    thirdChild = eval_operator(thirdChild, operator, eval(node->children[i]));
    i++;
  }
  
  return thirdChild; 
}


void print_result(mpc_result_t abstractSyntaxtTree) {
  long result = eval(abstractSyntaxtTree.output);
  printf("%li\n", result);
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
