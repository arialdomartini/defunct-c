#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <editline/readline.h>
#include <editline/history.h>

#define BUFFER_SIZE 8192

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


void print_result(mpc_result_t result) {
  mpc_ast_print(result.output);
  mpc_ast_delete(result.output);
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

    mpc_result_t result;
    if (mpc_parse("<stdin>", input, terms.program, &result)) {
      print_result(result);
    } else {
      print_error(result);
    }
  }

  teardown(terms);

  return 0;
}
