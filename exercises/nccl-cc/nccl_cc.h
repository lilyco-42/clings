/*
 * nccl_cc.h - New Concept C Language Compiler
 *
 * Shared data structures for the nccl-cc compiler.
 * This header grows incrementally with each lesson.
 */
#ifndef NCCL_CC_H
#define NCCL_CC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*===== Token =====*/
enum {
    TK_NUM,
    TK_IDENT,
    TK_KEYWORD,
    TK_PUNCT,
    TK_STRING,    /* L86: string literal */
    TK_EOF
};

typedef struct Token Token;
struct Token {
    int kind;
    Token *next;
    int val;
    char *loc;
    int len;
    int line;
};

/*===== AST Node =====*/
enum {
    ND_NUM,         /* integer literal (L73) */
    ND_VAR,         /* variable reference (L77) */
    ND_ASSIGN,      /* = (L78) */
    ND_NEG,         /* unary - (L74) */
    ND_NOT,         /* unary ! */
    ND_BITNOT,      /* unary ~ */
    ND_ADD,         /* + (L75) */
    ND_SUB,         /* - */
    ND_MUL,         /* * */
    ND_DIV,         /* / */
    ND_MOD,         /* % */
    ND_EQ,          /* == (L76) */
    ND_NE,          /* != */
    ND_LT,          /* < */
    ND_LE,          /* <= */
    ND_GT,          /* > */
    ND_GE,          /* >= */
    ND_AND,         /* && */
    ND_OR,          /* || */
    ND_BITAND,      /* & (bitwise) */
    ND_BITOR,       /* | (bitwise) */
    ND_BITXOR,      /* ^ (bitwise) */
    ND_SHL,         /* << left shift */
    ND_SHR,         /* >> right shift */
    ND_RETURN,      /* return (L73) */
    ND_IF,          /* if/else (L79) */
    ND_WHILE,       /* while (L80) */
    ND_FOR,         /* for (L80) */
    ND_DOWHILE,     /* do-while (L81) */
    ND_BREAK,       /* break (L81) */
    ND_CONTINUE,    /* continue (L81) */
    ND_EXPR_STMT,   /* expression statement (L78) */
    ND_VAR_DECL,    /* variable declaration (L77) */
    ND_BLOCK,       /* { ... } compound statement (L77) */
    ND_ADDR,        /* unary & (L88) */
    ND_DEREF,       /* unary * (L88) */
    ND_STR,         /* string literal (L86) */
    ND_MEMBER,      /* struct member access (L91) */
    ND_COND,        /* ternary ? : */
    ND_COMMA,       /* comma operator */
    ND_CAST,        /* (type)expr */
    ND_SWITCH,      /* switch statement */
    ND_CASE,        /* case label */
    ND_DEFAULT,     /* default label */
    ND_CALL,        /* function call (L82) */
    ND_FUNC,        /* function definition (L73) */
    ND_PROGRAM      /* top-level */
};

/* L87: Type system */
enum { TY_INT, TY_CHAR, TY_PTR, TY_ARRAY, TY_STRUCT };

typedef struct Type Type;
typedef struct Member Member;
struct Member {
    Member *next;
    char name[64];
    Type *ty;
    int offset;
};

struct Type {
    int kind;
    int size;       /* sizeof */
    int align;      /* alignment requirement (L95: ILP32 vs LP64) */
    Type *base;     /* pointer-to or array-of */
    int array_len;  /* array length */
    Member *members; /* L91: struct members */
};

/* Variable (local or global) */
typedef struct Var Var;
struct Var {
    Var *next;
    char name[64];
    int offset;     /* stack offset from s0 (locals only) */
    int is_global;  /* L85: 1 if global variable */
    int init_val;   /* L85: initial value for global */
    Type *ty;       /* L87: variable type */
};

/* L85: global variable list */
extern Var *globals;
extern int str_count;   /* L86: string literal counter */

typedef struct Node Node;
struct Node {
    int kind;
    Type *ty;       /* L95: type of this expression (set by add_type) */
    Node *lhs;      /* left child / expression */
    Node *rhs;      /* right child (for binary ops / assign) */
    Node *body;     /* block body / function body */
    Node *next;     /* linked list of statements */
    int val;        /* ND_NUM value */
    char name[64];  /* function/variable name */
    char *str_val;  /* L86: string literal value */
    int str_id;     /* L86: string literal ID */

    /* L91/L95: struct member access (resolved by add_type) */
    Member *member;

    /* if/while/for (L79-81) */
    Node *cond;     /* condition */
    Node *then;     /* if-then branch */
    Node *els;      /* if-else branch */
    Node *init;     /* for-init */
    Node *inc;      /* for-increment */

    /* variable ref (L77) */
    Var *var;

    /* ternary ? : */
    Node *cond_true;  /* ? branch */
    Node *cond_false; /* : branch */

    /* switch/case */
    Node *cases;    /* case list (linked via next) */
    int case_val;   /* case constant value */

    /* function call (L82) */
    Node *args;     /* argument list */
    int nargs;

    /* function def (L82) */
    Node *params;   /* parameter list (ND_VAR linked) */
    int nparams;
    Var *locals;    /* local variable list */
    int stack_size; /* bytes allocated for locals */
};

/*===== Function declarations =====*/

/* tokenize.c */
Token *tokenize(char *input);
/* emit a decoded string as a quoted assembly literal (re-escaped) */
void emit_escaped_string(const char *s);

/* parse.c */
Node *parse(Token *tokens);

/* codegen_rv.c */
void codegen_rv(Node *prog);

/* codegen_arm.c (L94) */
void codegen_arm(Node *prog);

/* codegen_a64.c: AArch64 (ARM64), LP64, ELF/Linux */
void codegen_a64(Node *prog);

/* codegen_x86.c */
void codegen_x86(Node *prog);
/* 1 = emit Mach-O (macOS) syntax, 0 = ELF (Linux).
 * Defaults to the host OS; overridable via -target=x86-mac/x86-linux. */
extern int x86_macos;

/* preprocess.c (L92) */
char *preprocess(char *input);
void preprocess_add_define(const char *name, const char *value);

/* utilities */
char *read_file(const char *path);

/* debug.c: developer tools */
void dump_tokens(Token *tok);
void dump_ast(Node *prog);

/* L87: type helpers */
Type *ty_int(void);
Type *ty_char(void);
Type *ptr_to(Type *base);
Type *array_of(Type *base, int len);

/* L95: target data model (ILP32 vs LP64).
 * Pointer size in bytes; set by main() BEFORE parsing, because sizeof,
 * struct layout and stack offsets are all computed at parse time. */
extern int target_ptr_size;

/* type.c: advanced type helpers */
int align_to(int n, int align);
int ptr_elem_size(Type *ty);
int is_pointer_like(Type *ty);
int types_compatible(Type *a, Type *b);
const char *type_name(Type *ty);
Member *find_member(Type *ty, const char *name);
Type *decay_array(Type *ty);
int type_sizeof(Type *ty);
Type *add_result_type(Type *lhs, Type *rhs);
Type *sub_result_type(Type *lhs, Type *rhs);
int is_integer_type(Type *ty);
Type *promote_type(Type *ty);
Type *common_type(Type *a, Type *b);

/* L95: annotate every AST node with its type (run after parse,
 * before codegen) — chibicc-style add_type pass */
void add_type(Node *node);

#endif /* NCCL_CC_H */
