#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <editline/readline.h>
#include <editline/history.h>

struct terms {
  mpc_parser_t* number;
  mpc_parser_t* symbol;
  mpc_parser_t* sexpression;
  mpc_parser_t* expression;
  mpc_parser_t* program;
};

enum { LVAL_NUMBER, LVAL_SYMBOL, LVAL_SEXPRESSION, LVAL_ERROR };
enum { LERR_DIVISON_BY_ZERO, LERR_UNKNOWN_OPERATOR, LERR_BAD_NUMBER };


typedef struct lval {
  int type;
  long double number;
  char* error;
  char* symbol;
  int nchildren;
  struct lval** children;
} lval;

lval* number_to_lval(long double number) {
  lval* val = malloc(sizeof(lval));
  val->type = LVAL_NUMBER;
  val->number = number;
  return val;
}


lval* error_to_lval(char* errorMessage) {
  lval* val = malloc(sizeof(lval));
  val->type = LVAL_ERROR;
  val->err = malloc(strlen(errorMessage) + 1);
  strcpy(val->err, errorMessage);
  return val;
}

lval* symbol_to_lval(char* symbol) {
  lval* val = malloc(sizeof(lval));
  val->type = LVAL_SYMBOL;
  val->sym = malloc(strlen(symbol) + 1);
  strcpy(val->sym, symbol);
  return val;
}

lval* sexpression_to_lval(void) {
  lval* val = malloc(sizeof(lval));
  val->type = LVAL_SEXPRESSION;
  val->nchildren = 0;
  val->children = NULL;
  return val;
}


void delele_lval(lval* val) {

  switch (val->type) {
  case LVAL_NUM: break;
    
  case LVAL_ERROR: free(val->err); break;
  case LVAL_SYMBOL: free(val->sym); break;
    
  case LVAL_SEXPRESSION:
    for (int i = 0; i < val->nchildren; i++) {
      delete_lval(v->children[i]);
    }
    free(v->children);
    break;
  }
  
  free(val);
}


lval* add_lval(lval* val, lval* newVal) {
  val->count++;
  val->children = realloc(val->children, sizeof(lval*) * val->nchildren);
  val->children[val->nchildren-1] = newVal;
  return val;
}

lval* pop_lval(lval* val, int index) {
  lval* found = val->cell[idex];
  
  memmove(&val->children[index], &val->children[index+1], sizeof(lval*) * (val->nchildren-index-1));
  
  val->nchildren--;
  
  val->children = realloc(val->children, sizeof(lval*) * val->nchildren);
  return found;
}


lval* take_lval(lval* val, int index) {
  lval* found = pop_lval(val, idex);
  del_lval(val);
  return found;
}


void print_lval(lval* val);

void print_lval_expr(lval* val, char open, char close) {
  putchar(open);
  for (int i = 0; i < val->count; i++) {
    
    print_lval(val->children[i]);
    
    if (i != (v->count-1)) {
      putchar(' ');
    }
  }
  putchar(close);
}


void print_lval(lval* val) {
  switch (val->type) {
  case LVAL_NUMBER:      printf("%li", val->num); break;
  case LVAL_ERROR:       printf("Error: %s", val->err); break;
  case LVAL_SYMBOL:      printf("%s", val->sym); break;
  case LVAL_SEXPRESSION: lval_expr_print(val, '(', ')'); break;
  }
}

void println_lval(lval* val) { 
  print_lval(v); 
  putchar('\n'); 
}


lval* builtin_op(lval* a, char* op) {
  
  for (int i = 0; i < a->count; i++) {
    if (a->children[i]->type != LVAL_NUMBER) {
      delete_lval(a);
      return lval_err("Cannot operate on non-number!");
    }
  }
 
  lval* x = lval_pop(a, 0);
  
  if ((strcmp(op, "-") == 0) && a->count == 0) { x->num = -x->num; }
  
  while (a->count > 0) {
  
    lval* y = lval_pop(a, 0);

  if      (strcmp(operator, "+") == 0) { x.number = sum(x.number,y.number); }
  else if (strcmp(operator, "-") == 0) { x.number = subtract(x.number,y.number); }
  else if (strcmp(operator, "*") == 0) { x.number = multiply(x.number,y.number); }
  else if (strcmp(operator, "/") == 0) { 
    if (y->num == 0) {
      delete_lval(x); delete_lval(y);
      x = lval_err("Division By Zero.");
      break;
    }
    x.number = divide(x.number,y.number);
  }
  
  delete_lval(a);
  return x;
} 


struct terms defineGrammar() {

  struct terms t = {
    .number = mpc_new("number"),
    .mathOperator = mpc_new("symbol"),
    .sexpression = mpc_new("sexpression"),
    .expression = mpc_new("expression"),
    .program = mpc_new("program")
  };

  mpca_lang(MPCA_LANG_DEFAULT,
  "\
    number       :  /-?[0-9]+/ ;                                       \
    symbol       :  '+' | '-' | '*' | '/' ;                            \
    sexpression  :  '(' <expression> ')' ;                             \
    expression   :  <number> | <symbol> | <sexpression>;               \
    program      :  /^/ <expression>* /$/ ;                            \
  ",
    t.number, t.symbol, t.sexpression, t.expression, t.program);

  return t;
}

void teardown(struct terms t) {
  mpc_cleanup(4, t.number, t.symbol, t.sexpression, t.expression, t.program);
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
