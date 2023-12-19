#ifndef LAROC_TYPEDEF_H
#define LAROC_TYPEDEF_H

typedef enum Kwd Kwd;
typedef enum Punct Punct;
typedef enum TokenKind TokenKind;
typedef struct Number Number;
typedef struct Token Token;

typedef enum CTypeKind CTypeKind;
typedef enum ExprKind ExprKind;
typedef enum ExprPrecedence ExprPrecedence;
typedef enum StmtKind StmtKind;
typedef struct CType CType;
typedef struct Declaration Declaration;
typedef struct Declarator Declarator;
typedef struct Expr Expr;
typedef struct ParseCtx ParseCtx;
typedef struct Stmt Stmt;
typedef struct Symbol Symbol;
typedef struct SymTableEntry SymTableEntry;
typedef struct SymTable SymTable;
typedef struct TranslationUnit TranslationUnit;

typedef enum IRInstKind IRInstKind;
typedef enum IRTypeKind IRTypeKind;
typedef enum ValKind ValKind;
typedef struct Block Block;
typedef struct Func Func;
typedef struct IRInst IRInst;
typedef struct IRType IRType;
typedef struct Module Module;
typedef struct Value Value;

typedef struct IRGenCtx IRGenCtx;

#endif
