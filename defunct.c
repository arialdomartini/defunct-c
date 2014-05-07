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

int main(int argc, char** argv) {

  puts(">> Defunct v0.3");

  struct terms t = defineGrammar();

  while (1) {
    char* input = readline(": ");

    mpc_result_t r;
    if (mpc_parse("<stdin>", input, t.program, &r)) {
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
  }

  mpc_cleanup(4, t.number, t.mathOperator, t.sExpression, t.program);

  return 0;
}
