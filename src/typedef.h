#ifndef LAROC_TYPEDEF_H
#define LAROC_TYPEDEF_H

typedef enum Kwd Kwd;
typedef enum Punct Punct;
typedef struct Number Number;
typedef struct Token Token;
typedef enum TokenKind TokenKind;

typedef struct Declaration Declaration;
typedef struct Declarator Declarator;
typedef struct Expr Expr;
typedef enum ExprKind ExprKind;
typedef enum ExprPrecedence ExprPrecedence;
typedef struct Stmt Stmt;
typedef enum StmtKind StmtKind;
typedef struct Symbol Symbol;
typedef struct SymTable SymTable;
typedef struct SymTableEntry SymTableEntry;
typedef struct Type Type;
typedef enum TypeKind TypeKind;

#endif
