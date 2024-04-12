#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "lex/kwd.h"
#include "lex/punct.h"
#include "lex/token.h"
#include "parse/decl.h"
#include "parse/expr.h"
#include "parse/parse.h"
#include "parse/stmt.h"
#include "sema/decl.h"
#include "sema/expr.h"
#include "sema/stmt.h"
#include "sema/symbol.h"
#include "sema/type.h"
#include "util/diag.h"

static int parseLabel(ParseCtx *ctx, const Token *begin, Stmt *stmt);
static int parseIfStmt(ParseCtx *ctx, const Token *begin, Stmt *stmt);
static int parseGotoStmt(const Token *begin, Stmt *stmt);
static int parseReturnStmt(ParseCtx *ctx, const Token *begin, Stmt *stmt);

int parseStmt(ParseCtx *ctx, const Token *begin, Stmt *stmt) {
  const Token *p = begin;
  int n;

  if (tokenIsPunct(p, PUNCT_SEMICOLON)) {
    stmt->kind = STMT_EMPTY;
    return 1;
  }

  if (begin->kind == TOK_IDENT && tokenIsPunct(begin + 1, PUNCT_COLON))
    return parseLabel(ctx, p, stmt);

  if (tokenIsPunct(p, PUNCT_BRACE_L))
    return parseCmpdStmt(ctx, p, stmt);

  if (tokenIsKwd(p, KWD_IF))
    return parseIfStmt(ctx, p, stmt);

  if (tokenIsKwd(p, KWD_GOTO))
    return parseGotoStmt(p, stmt);
  if (tokenIsKwd(p, KWD_RETURN))
    return parseReturnStmt(ctx, p, stmt);

  stmt->decl = calloc(1, sizeof(Declaration));
  if ((n = parseDeclaration(ctx, p, stmt->decl)) == 0) {
    free(stmt->decl);
    stmt->decl = NULL;
  } else {
    p += n;
    stmt->kind = STMT_DECL;
    return p - begin;
  }

  if ((n = parseExpr(ctx, begin, EXPR_PREC_ALL, &stmt->expr1)) != 0) {
    p += n;
    stmt->kind = STMT_EXPR;

    if (!tokenIsPunct(p, PUNCT_SEMICOLON)) {
      emitDiagnostic(p->loc, "Expect ';'");
    }
    return p + 1 - begin;
  }

  return 0;
}

int parseCmpdStmt(ParseCtx *ctx, const Token *begin, Stmt *stmt) {
  const Token *p = begin;
  int n;

  if (!tokenIsPunct(p, PUNCT_BRACE_L)) {
    emitDiagnostic(p->loc, "expect left brace");
  }
  p++;
  stmt->kind = STMT_CMPD;

  SymTable *savedSymtab = ctx->symtab;
  if (stmt->symtab) {
    assert(ctx->symtab == stmt->symtab);
  } else {
    ctx->symtab = stmt->symtab = newSymTable(ctx->symtab);
  }

parse_compound_statement_begin:
  if (tokenIsPunct(p, PUNCT_BRACE_R)) {
    p++;
    ctx->symtab = savedSymtab;
    return p - begin;
  }

  Stmt *childStmt = calloc(1, sizeof(Stmt));
  if ((n = parseStmt(ctx, p, childStmt)) == 0) {
    free(childStmt);
    emitDiagnostic(p->loc, "expect declaration or statement");
  }
  p += n;
  arrput(stmt->children, childStmt);

  goto parse_compound_statement_begin;
}

static int parseLabel(ParseCtx *ctx, const Token *begin, Stmt *stmt) {
  assert(begin->kind == TOK_IDENT && tokenIsPunct(begin + 1, PUNCT_COLON));
  stmt->kind = STMT_LABEL;
  stmt->label = begin->ident;
  stmt->labelLoc = begin->loc;

  if (symTableGetShallow(ctx->func->labelTable, stmt->label)) {
    emitDiagnostic(begin->loc, "label %s already exist", stmt->label);
  }
  symTablePut(ctx->func->labelTable, newSymbol(stmt->label, NULL));

  return 2;
}

static int parseIfStmt(ParseCtx *ctx, const Token *begin, Stmt *stmt) {
  const Token *p = begin;
  int n;

  assert(tokenIsKwd(p, KWD_IF));
  p++;
  stmt->kind = STMT_IF;

  if (!tokenIsPunct(p, PUNCT_PAREN_L)) {
    emitDiagnostic(p->loc, "Expect '('");
  }
  p++;

  if ((n = parseExpr(ctx, p, EXPR_PREC_ALL, &stmt->expr1)) == 0) {
    emitDiagnostic(p->loc, "Expect expression");
  }
  p += n;

  if (!typeIsScarlar(stmt->expr1->ty)) {
    emitDiagnostic(p->loc,
                   "The controlling expression of an if statement shall have "
                   "scalar type");
  }

  if (!tokenIsPunct(p, PUNCT_PAREN_R)) {
    emitDiagnostic(p->loc, "Expect ')'");
  }
  p++;

  Stmt *s = calloc(1, sizeof(Stmt));
  if ((n = parseStmt(ctx, p, s)) == 0) {
    emitDiagnostic(p->loc, "Expect statement");
  }
  p += n;
  stmt->stmt1 = s;

  if (!tokenIsKwd(p, KWD_ELSE))
    return p - begin;
  p++;

  s = calloc(1, sizeof(Stmt));
  if ((n = parseStmt(ctx, p, s)) == 0) {
    emitDiagnostic(p->loc, "Expect statement");
  }
  stmt->stmt2 = s;
  return p + n - begin;
}

static int parseGotoStmt(const Token *begin, Stmt *stmt) {
  const Token *p = begin;
  assert(tokenIsKwd(begin, KWD_GOTO));
  stmt->kind = STMT_GOTO;
  p++;

  if (p->kind != TOK_IDENT) {
    emitDiagnostic(p->loc, "Expect identifier");
  }
  stmt->label = p->ident;
  stmt->labelLoc = p->loc;
  p++;

  if (!tokenIsPunct(p, PUNCT_SEMICOLON)) {
    emitDiagnostic(p->loc, "Expect semicolon");
  }
  p++;
  return p - begin;
}

static int parseReturnStmt(ParseCtx *ctx, const Token *begin, Stmt *stmt) {
  const Token *p = begin;
  int n;

  assert(tokenIsKwd(p, KWD_RETURN));
  p++;
  stmt->kind = STMT_RETURN;

  if (ctx->func->decltors[0]->ty->func.ret->kind != TYPE_VOID) {
    if ((n = parseExpr(ctx, p, EXPR_PREC_ALL, &stmt->expr1)) == 0) {
      emitDiagnostic(p->loc, "Expect expression");
    }
    p += n;
  }

  if (!tokenIsPunct(p, PUNCT_SEMICOLON)) {
    emitDiagnostic(p->loc, "Expect semicolon");
  }
  return p + 1 - begin;
}
