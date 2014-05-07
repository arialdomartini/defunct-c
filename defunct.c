#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <editline/readline.h>
#include <editline/history.h>

#define BUFFER_SIZE 8192

int main(int argc, char** argv) {

  puts(">> Defunct v0.3");

  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* MathOperator = mpc_new("operator");
  mpc_parser_t* SExpression = mpc_new("sexpression");
  mpc_parser_t* Lisp = mpc_new("program");

  mpca_lang(MPCA_LANG_DEFAULT,
  "\
    number       :  /-?[0-9]+/ ;                                   \
    operator     :  '+' | '-' | '*' | '/' ;                        \
    sexpression  :  <number> | '[' <operator> <sexpression>+ ']' ; \
    program      :  /^/ <operator> <sexpression>+ /$/ ;            \
  ",
   Number, MathOperator, SExpression, Lisp);


  while (1) {
    char* input = readline(": ");

    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lisp, &r)) {
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
  }

  mpc_cleanup(4, Number, MathOperator, SExpression, Lisp);

  return 0;
}
