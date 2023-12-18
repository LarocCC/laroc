#ifndef LAROC_TYPEDEF_H
#define LAROC_TYPEDEF_H

typedef enum Kwd Kwd;
typedef enum Punct Punct;
typedef struct Number Number;
typedef struct Token Token;
typedef enum TokenKind TokenKind;

typedef struct CType CType;
typedef enum CTypeKind CTypeKind;
typedef struct Declaration Declaration;
typedef struct Declarator Declarator;
typedef struct Expr Expr;
typedef enum ExprKind ExprKind;
typedef enum ExprPrecedence ExprPrecedence;
typedef struct ParseCtx ParseCtx;
typedef struct Stmt Stmt;
typedef enum StmtKind StmtKind;
typedef struct Symbol Symbol;
typedef struct SymTable SymTable;
typedef struct SymTableEntry SymTableEntry;
typedef struct TranslationUnit TranslationUnit;

typedef struct Func Func;
typedef struct Module Module;
typedef struct Val Val;
typedef enum ValKind ValKind;
typedef struct IRType IRType;
typedef enum IRTypeKind IRTypeKind;

typedef struct IRGenCtx IRGenCtx;

#endif
