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
#include "util/diag.h"

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
                           Declarator *decltor, bool allowAbstract,
                           bool onlyAbstract);
static int parsePointerDeclarator(ParseCtx *ctx, const Token *begin,
                                  Declarator *decltor, bool allowAbstract,
                                  bool onlyAbstract);
static int parseArrayDeclarator(ParseCtx *ctx, const Token *begin,
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
  if ((n = parseSpecifier(ctx, p, spec)) == 0) {
    free(spec);
    return 0;
  }
  p += n;

  bool allowFuncDef = !ctx->func;

  while (!tokenIsPunct(p, PUNCT_SEMICOLON)) {
    // Declarator
    Declarator *decltor = calloc(1, sizeof(Declarator));
    if ((n = parseDeclarator(ctx, p, decltor, false, false)) == 0) {
      free(decltor);
      return 0;
    }
    p += n;
    decltor->ty = fillUntyped(decltor->ty, spec);
    if (decltor->ty->kind != TYPE_FUNC)
      decltor->ty->attr |= TYPE_ATTR_LVALUE;

    // Update symbol table.
    if (symTableGetShallow(ctx->symtab, decltor->ident)) {
      emitDiagnostic(decltor->loc, "Symbol %s already exist", decltor->ident);
    }
    Symbol *sym = newSymbol(decltor->ident, decltor->ty);
    symTablePut(ctx->symtab, sym);

    // Compound statement
    if (tokenIsPunct(p, PUNCT_BRACE_L)) {
      if (!allowFuncDef) {
        emitDiagnostic(p->loc, "Function defination is not allowed here");
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
        emitDiagnostic(p->loc, "Expect expression");
      }
      p += n;
    }

    arrput(decltion->decltors, decltor);

    if (!tokenIsPunct(p, PUNCT_COMMA)) {
      break;
    }
    p++;
  }

  if (!tokenIsPunct(p, PUNCT_SEMICOLON)) {
    emitDiagnostic(p->loc, "Expect ';'");
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
      emitDiagnostic(param->loc, "Parameter %s already exist", param->ident);
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
                           Declarator *decltor, bool allowAbstract,
                           bool onlyAbstract) {
  if (onlyAbstract)
    assert(allowAbstract);
  const Token *p = begin;

  if (tokenIsPunct(p, PUNCT_PAREN_L)) {
    p++;
    p += parseDeclarator(ctx, p, decltor, allowAbstract, onlyAbstract);
    if (!tokenIsPunct(p, PUNCT_PAREN_R)) {
      emitDiagnostic(p->loc, "Missing ')'");
    }
    p++;
    goto parse_declarator_suffix_begin;
  }

  if (tokenIsPunct(p, PUNCT_STAR)) {
    return parsePointerDeclarator(ctx, p, decltor, allowAbstract, onlyAbstract);
  }

  if (p->kind == TOK_IDENT) {
    if (onlyAbstract)
      emitDiagnostic(p->loc, "Unexpected identifier '%s'", p->ident);
    decltor->ty = newCType(TYPE_UNTYPED, TYPE_ATTR_NONE);
    decltor->ident = p->ident;
    decltor->loc = p->loc;
    p++;
  } else {
    if (!allowAbstract)
      emitDiagnostic(p->loc, "Expect identifier");
    decltor->ty = newCType(TYPE_UNTYPED, TYPE_ATTR_NONE);
  }

parse_declarator_suffix_begin:
  // Array declarator
  if (tokenIsPunct(p, PUNCT_BRACKET_L)) {
    p += parseArrayDeclarator(ctx, p, decltor);
    goto parse_declarator_suffix_begin;
  }
  // Function declarator
  if (tokenIsPunct(p, PUNCT_PAREN_L)) {
    p += parseFunctionDeclarator(ctx, p, decltor);
    goto parse_declarator_suffix_begin;
  }

  return p - begin;
}

/// Parse a pointer declarator, starting from the asterisk.
static int parsePointerDeclarator(ParseCtx *ctx, const Token *begin,
                                  Declarator *decltor, bool allowAbstract,
                                  bool onlyAbstract) {
  const Token *p = begin;
  assert(tokenIsPunct(p, PUNCT_STAR));
  p++;

  p += parseDeclarator(ctx, p, decltor, allowAbstract, onlyAbstract);
  CType *ptrTy = newCType(TYPE_PTR, TYPE_ATTR_NONE);
  ptrTy->ptr.inner = newCType(TYPE_UNTYPED, TYPE_ATTR_LVALUE);
  decltor->ty = fillUntyped(decltor->ty, ptrTy);

  return p - begin;
}

/// Parse an array declarator, starting from the left bracket.
static int parseArrayDeclarator(ParseCtx *ctx, const Token *begin,
                                Declarator *decltor) {
  const Token *p = begin;
  assert(tokenIsPunct(p, PUNCT_BRACKET_L));
  p++;

  CType *arrTy = newCType(TYPE_ARRAY, TYPE_ATTR_NONE);
  arrTy->arr.inner = newCType(TYPE_UNTYPED, TYPE_ATTR_NONE);
  if (!tokenIsPunct(p, PUNCT_BRACKET_R)) {
    // TODO: type-qualifier-list
    // TODO: static
    // TODO: *
    p += parseExpr(ctx, p, EXPR_PREC_ALL, &arrTy->arr.size);
  }
  if (!tokenIsPunct(p, PUNCT_BRACKET_R))
    emitDiagnostic(p->loc, "Expect ']'");
  p++;

  decltor->ty = fillUntyped(decltor->ty, arrTy);
  return p - begin;
}

/// Parse a function declarator, starting from the left paren.
static int parseFunctionDeclarator(ParseCtx *ctx, const Token *begin,
                                   Declarator *decltor) {
  const Token *p = begin;
  assert(tokenIsPunct(p, PUNCT_PAREN_L));
  p++;

  CType *funcTy = newCType(TYPE_FUNC, TYPE_ATTR_NONE);
  funcTy->func.ret = newCType(TYPE_UNTYPED, TYPE_ATTR_NONE);
  if (tokenIsPunct(p, PUNCT_PAREN_R))
    goto parse_parameter_list_end;

parse_parameter_list_begin:;
  Declarator *paramDecltor = calloc(1, sizeof(Declarator));
  p += parseParameter(ctx, p, paramDecltor);

  arrput(funcTy->func.params, paramDecltor);

  if (tokenIsPunct(p, PUNCT_COMMA)) {
    p++;
    // Parse next parameter.
    goto parse_parameter_list_begin;
  }

parse_parameter_list_end:
  if (!tokenIsPunct(p, PUNCT_PAREN_R)) {
    emitDiagnostic(p->loc, "Expect ')'");
  }
  p++;

  decltor->ty = fillUntyped(decltor->ty, funcTy);
  return p - begin;
}

/// Parse a parameter.
static int parseParameter(ParseCtx *ctx, const Token *begin,
                          Declarator *paramDecltor) {
  const Token *p = begin;
  int n;

  // Specifier of the parameter
  CType *paramSpec = calloc(1, sizeof(CType));
  if ((n = parseSpecifier(ctx, p, paramSpec)) == 0) {
    free(paramSpec);
    emitDiagnostic(p->loc, "Expect type specifier");
  }
  p += n;

  // Declarator of the parameter
  p += parseDeclarator(ctx, p, paramDecltor, true, false);

  paramDecltor->ty->attr |= TYPE_ATTR_LVALUE;
  paramDecltor->ty = fillUntyped(paramDecltor->ty, paramSpec);

  return p - begin;
}

int parseTypeName(ParseCtx *ctx, const Token *begin, CType **ty) {
  const Token *p = begin;
  int n;

  // Specifier
  CType *spec = calloc(1, sizeof(CType));
  if ((n = parseSpecifier(ctx, p, spec)) == 0) {
    free(spec);
    return 0;
  }
  p += n;

  // Abstract declarator
  Declarator abstractDecltor;
  memset(&abstractDecltor, 0, sizeof(Declarator));
  p += parseDeclarator(ctx, p, &abstractDecltor, true, true);

  *ty = fillUntyped(abstractDecltor.ty, spec);

  return p - begin;
}
