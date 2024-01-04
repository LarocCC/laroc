#ifndef LAROC_TYPEDEF_H
#define LAROC_TYPEDEF_H

typedef struct CLIOpt CLIOpt;

typedef enum Kwd Kwd;
typedef enum Punct Punct;
typedef enum TokenKind TokenKind;
typedef struct Number Number;
typedef struct Token Token;

typedef enum CTypeAttr CTypeAttr;
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
typedef struct IRBlock IRBlock;
typedef struct IRCtx IRCtx;
typedef struct IRFunc IRFunc;
typedef struct IRInst IRInst;
typedef struct IRType IRType;
typedef struct Module Module;
typedef struct Value Value;

typedef struct IRGenCtx IRGenCtx;

typedef enum OperandKind OperandKind;
typedef enum Reg Reg;
typedef enum RVInstKind RVInstKind;
typedef struct FrameObject FrameObject;
typedef struct ObjectFile ObjectFile;
typedef struct Operand Operand;
typedef struct RVBlock RVBlock;
typedef struct RVCtx RVCtx;
typedef struct RVFunc RVFunc;
typedef struct RVInst RVInst;

#endif
