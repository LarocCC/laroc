#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "lex/punct.h"
#include "lex/token.h"
#include "parse/expr.h"
#include "parse/parse.h"
#include "parse/stmt.h"
#include "parse/type.h"
#include "sema/decl.h"
#include "sema/stmt.h"
#include "sema/symbol.h"
#include "sema/type.h"

// A declaration consist of specifiers, declarators and initializers.
//
//          |--------------------(3)--------------------|
//     |-(2)--------------------------------------------|
// int val, (*cmp)(const char *a, const char *b) = strcmp;
// |-|      |-(4)------------------------------| |-(7)--|
//  ^              |-----------------(5)------|    |-(8)|
// (1)             |----(6)----|
//
// A function defination consist of specifiers, a declarator and function body.
//
//     |----------------(4)---------------|
// int strcmp(const char *a, const char *b) { ... };
// |-|        |-----------------(5)------|  |-(9)-|
//  ^         |----(6)----|
// (1)
//
// The following table shows the relationship between C99 grammar element,
// parser function and the struct in AST. The parser functions parse the exact
// tokens shown in the graph above.
//
//     C99 Grammar                      Parser                     Struct
// (0) declaration/function-defination  parseDeclaration()         Declaration
// (1) specifiers                       parseSpecifier()           CType
// (2) list of init-declarator          -                          -
// (3) init-declarator                  -                          Declarator
// (4) declarator                       parseDeclarator()          Declarator
// (6) list of parameter                -                          -
// (6) parameter                        parseParameter()           Declarator
// (7) initializer                      -                          Expr
// (8) expression                       parseExpr()                Expr
// (9) compound statement               parseFunctionDefination()  Stmt
//
// Take "const Token *begin" as an example, to construct a Declarator struct,
//
// 1. Specifiers are parsed into CType by parseSpecifier(). That is,
// "const Token" is parsed as a CType.
//
// 2. A C grammar declarator is parsed into a Declarator by parseDeclarator().
// The type field of the Declarator struct will be TYPE_UNTYPED or contains
// TYPE_UNTYPED indirectly. In the example, the C grammar declarator is
//  "*begin". It will be parsed as a Declarator struct with identifier "begin"
// and type "pointer to TYPE_UNTYPED".
//
// 3. Fill the TYPE_UNTYPED with the specifiers in step 1, finalizing the type
// in the Declarator struct. In the example, the type after this step will be
// "pointer to const Token".
//
// 4. If an initializer exists, it will be parsed and added to the Declarator
// struct. There is no initializer in the example.
//
// Another more complicated example is from the first graph of this comment
// block: "int val, (*cmp)(const char *a, const char *b) = strcmp;"
//
// 1. Parse "int" as the specifier.
//
// 2. Parse "val" as a Declarator struct with identifier "val" and type
// TYPE_UNTYPED.
//
// 3. See ",". Fill TYPE_UNTYPED with the specifier. The current Declarator
// struct will be identifier "val" and type "int". Add the Declarator struct to
// the current Declaration struct. Then start parsing the next declarator.
//
// 4. After seeing "(*cmp)", the Declarator struct will declare an identifier
// "cmp" with "pointer to TYPE_UNTYPED".
//
// 5. After seeing "(*cmp)(const char *a, const char *b)", the type will be
// "pointer to [function (pointer to const char, pointer to const char)
// returning TYPE_UNTYPED]".
//
// 6. The type is filled to "pointer to [function (pointer to const char,
// pointer to const char) returning int]".
//
// 7. See "=". Parse exression "strcmp" parsed as an initializer.
//
// 8. See ";". End of parsing declaration.
//
// TODO: Parsing abstract declarator in C grammar is not supported yet. It
// should be supported in parseDeclarator()?

static int parseFunctionDefination(ParseCtx *ctx, const Token *begin,
                                   Declaration *decltion);
static int parseDeclarator(ParseCtx *ctx, const Token *begin,
                           Declarator *decltor);
static int parsePointerDeclarator(ParseCtx *ctx, const Token *begin,
                                  Declarator *decltor);
static int parseFunctionDeclarator(ParseCtx *ctx, const Token *begin,
                                   Declarator *decltor);
static int parseParameter(ParseCtx *ctx, const Token *begin,
                          Declarator *paramDecltor);

int parseDeclaration(ParseCtx *ctx, const Token *begin, Declaration *decltion) {
  const Token *p = begin;
  int n;

  // Specifier
  CType *spec = calloc(1, sizeof(CType));
  if ((n = parseSpecifier(p, spec)) == 0) {
    free(spec);
    return 0;
  }
  p += n;

  bool allowFuncDef = !ctx->func;

parse_declarator_list_begin:;
  // Declarator
  Declarator *decltor = calloc(1, sizeof(Declarator));
  if ((n = parseDeclarator(ctx, p, decltor)) == 0) {
    free(decltor);
    return 0;
  }
  p += n;
  decltor->ty = fillUntyped(decltor->ty, spec);
  if (decltor->ty->kind != TYPE_FUNC)
    decltor->ty->attr |= TYPE_ATTR_LVALUE;

  // Update symbol table.
  if (symTableGetShallow(ctx->symtab, decltor->ident)) {
    printf("symbol %s already exist\n", decltor->ident);
    exit(1);
  }
  Symbol *sym = newSymbol(decltor->ident, decltor->ty);
  symTablePut(ctx->symtab, sym);

  // Compound statement
  if (tokenIsPunct(p, PUNCT_BRACE_L)) {
    if (!allowFuncDef) {
      printf("function defination is not allowed here\n");
      exit(1);
    }
    arrput(decltion->decltors, decltor);
    p += parseFunctionDefination(ctx, p, decltion);
    return p - begin;
  }
  allowFuncDef = false;

  // Initializer
  if (tokenIsPunct(p, PUNCT_EQ_ASSIGN)) {
    p++;
    if ((n = parseExpr(ctx, p, EXPR_PREC_ASSIGN, &decltor->init)) == 0) {
      printf("expect expression\n");
      exit(1);
    }
    p += n;
  }

  arrput(decltion->decltors, decltor);

  if (tokenIsPunct(p, PUNCT_COMMA)) {
    p++;
    // Parse next declarator.
    goto parse_declarator_list_begin;
  }

  if (!tokenIsPunct(p, PUNCT_SEMICOLON)) {
    printf("missing semicolon\n");
    exit(1);
  }
  p++;
  return p - begin;
}

/// Parse a function defination, starting from the left brace, i.e. the function
/// body.
static int parseFunctionDefination(ParseCtx *ctx, const Token *begin,
                                   Declaration *decltion) {
  assert(tokenIsPunct(begin, PUNCT_BRACE_L));
  Declarator *decltor = decltion->decltors[0];
  const Token *p = begin;

  decltion->funcDef = calloc(1, sizeof(Stmt));
  decltion->labelTable = newSymTable(NULL);
  ctx->symtab = decltion->funcDef->symtab = newSymTable(ctx->symtab);
  ctx->func = decltion;

  for (int i = 0; i < arrlen(decltor->ty->func.params); i++) {
    Declarator *param = decltor->ty->func.params[i];
    if (symTableGetShallow(ctx->symtab, param->ident)) {
      printf("parameter %s already exist\n", param->ident);
      exit(1);
    }
    symTablePut(ctx->symtab, newSymbol(param->ident, param->ty));
  }

  p += parseCmpdStmt(ctx, p, decltion->funcDef);

  ctx->func = NULL;
  ctx->symtab = ctx->symtab->parent;
  return p - begin;
}

/// Parse a C grammar declarator.
static int parseDeclarator(ParseCtx *ctx, const Token *begin,
                           Declarator *decltor) {
  const Token *p = begin;

  if (tokenIsPunct(p, PUNCT_STAR)) {
    return parsePointerDeclarator(ctx, p, decltor);
  }

  if (p->kind == TOK_IDENT) {
    decltor->ty = newCType(TYPE_UNTYPED, TYPE_ATTR_NONE);
    decltor->ident = p->ident;
    p++;
  }

parse_declarator_suffix_begin:
  // Function declarator
  if (tokenIsPunct(p, PUNCT_PAREN_L)) {
    p += parseFunctionDeclarator(ctx, p, decltor);
    goto parse_declarator_suffix_begin;
  }

  return p - begin;
}

/// Parse a pointer declarator, starting from the asterisk.
static int parsePointerDeclarator(ParseCtx *ctx, const Token *begin,
                                  Declarator *decltor) {
  const Token *p = begin;
  assert(tokenIsPunct(p, PUNCT_STAR));
  p++;

  CType *ptrTy = newCType(TYPE_PTR, TYPE_ATTR_NONE);
  p += parseDeclarator(ctx, p, decltor);
  ptrTy->ptr.inner = decltor->ty;
  ptrTy->ptr.inner->attr |= TYPE_ATTR_LVALUE;
  decltor->ty = ptrTy;

  return p - begin;
}

/// Parse a function declarator, starting from the left paren.
static int parseFunctionDeclarator(ParseCtx *ctx, const Token *begin,
                                   Declarator *decltor) {
  const Token *p = begin;
  assert(tokenIsPunct(p, PUNCT_PAREN_L));
  p++;

  CType *funcTy = newCType(TYPE_FUNC, TYPE_ATTR_NONE);
  funcTy->func.ret = decltor->ty;
  decltor->ty = funcTy;
  if (tokenIsPunct(p, PUNCT_PAREN_R))
    goto parse_parameter_list_end;

parse_parameter_list_begin:;
  Declarator *paramDecltor = calloc(1, sizeof(Declarator));
  p += parseParameter(ctx, p, paramDecltor);

  arrput(decltor->ty->func.params, paramDecltor);

  if (tokenIsPunct(p, PUNCT_COMMA)) {
    p++;
    // Parse next parameter.
    goto parse_parameter_list_begin;
  }

parse_parameter_list_end:
  if (!tokenIsPunct(p, PUNCT_PAREN_R)) {
    printf("expect right paren\n");
    exit(1);
  }
  p++;

  return p - begin;
}

/// Parse a parameter.
static int parseParameter(ParseCtx *ctx, const Token *begin,
                          Declarator *paramDecltor) {
  const Token *p = begin;
  int n;

  // Specifier of the parameter
  CType *paramSpec = calloc(1, sizeof(CType));
  if ((n = parseSpecifier(p, paramSpec)) == 0) {
    free(paramSpec);
    printf("expect type specifier\n");
    exit(1);
  }
  p += n;

  // Declarator of the parameter
  if ((n = parseDeclarator(ctx, p, paramDecltor)) == 0) {
    free(paramDecltor);
    printf("expect declarator\n");
    exit(1);
  }
  p += n;

  paramDecltor->ty->attr |= TYPE_ATTR_LVALUE;
  paramDecltor->ty = fillUntyped(paramDecltor->ty, paramSpec);

  return p - begin;
}
