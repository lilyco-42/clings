/*
 * parse.c - Parser for nccl-cc
 *
 * Lesson 73: return <number>
 * Lesson 74: unary -, !, ~
 * Lesson 75: binary +, -, *, /, %
 * Lesson 76: comparison ==, !=, <, <=, >, >= and logic &&, ||
 * Lesson 77: local variables, variable declarations
 * Lesson 78: assignment expressions, expression statements, blocks
 */
#include <stdarg.h>

#include "nccl_cc.h"

static Token *tok;
static Var *cur_locals;      /* local vars of current function */
Var *globals = NULL;         /* L85: global variable list */
int str_count = 0;           /* L86: string literal counter */
static int parse_errors = 0; /* error counter for recovery mode */

/* forward declarations */
static Node *new_node(int kind);
static Node *new_binary(int kind, Node *lhs, Node *rhs);
static Node *new_num(int val);
static Var *find_var(const char *name, int len);
static int consume(const char *s);

static int equal(const char *s) { return tok->len == (int)strlen(s) && strncmp(tok->loc, s, tok->len) == 0; }

/* Print a parse error with context (token location and expected info).
 * A hard cap bounds error cascades: after enough noise the rest of
 * the diagnostics would be garbage anyway, so give up cleanly. */
static void error_token(Token *t, const char *fmt, ...) {
    fprintf(stderr, "\033[1;31merror\033[0m at L%d: ", t->line);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    if (t->kind == TK_EOF)
        fprintf(stderr, "\n  got: end of file\n");
    else
        fprintf(stderr, "\n  got: '%.*s'\n", t->len, t->loc);
    parse_errors++;
    if (parse_errors >= 20) {
        fprintf(stderr, "too many errors, giving up\n");
        exit(1);
    }
}

/* Skip tokens until we find ';' or '}' (error recovery point) */
static void skip_to_recovery(void) {
    while (tok->kind != TK_EOF) {
        if (equal(";")) {
            tok = tok->next;
            return;
        }
        if (equal("}")) return; /* don't consume '}' */
        tok = tok->next;
    }
}

/* Non-fatal error: report and recover to next statement */
static Node *error_recover(const char *msg) {
    error_token(tok, "%s", msg);
    skip_to_recovery();
    return new_node(ND_BLOCK); /* return empty node */
}

/* Consume the expected token or report and RECOVER - never exits.
 * For a missing ';' we skip ahead to a statement boundary so a single
 * typo doesn't drown the user in cascade errors; for any other token
 * we stay put and let the caller's grammar continue from here (the
 * global error cap in error_token bounds the cascade, and every
 * list-parsing loop is EOF-guarded via until()). */
static void skip(const char *s) {
    if (consume(s)) return;
    error_token(tok, "expected '%s'", s);
    if (strcmp(s, ";") == 0) skip_to_recovery();
}

/* Loop guard for list parsing: keep going while the closing token has
 * not arrived, but STOP (with a diagnostic) at end of file. Every
 * `while (!equal("}"))`-style loop must use this so that an unclosed
 * brace can never hang the compiler. */
static int until(const char *end) {
    if (tok->kind == TK_EOF) {
        error_token(tok, "unexpected end of file: missing '%s'", end);
        return 0;
    }
    return !equal(end);
}

static int consume(const char *s) {
    if (equal(s)) {
        tok = tok->next;
        return 1;
    }
    return 0;
}

static Node *new_node(int kind) {
    Node *n = calloc(1, sizeof(Node));
    n->kind = kind;
    return n;
}

static Node *new_binary(int kind, Node *lhs, Node *rhs) {
    Node *n = new_node(kind);
    n->lhs = lhs;
    n->rhs = rhs;
    return n;
}

static Node *new_num(int val) {
    Node *n = new_node(ND_NUM);
    n->val = val;
    return n;
}

/* Find variable by name in current scope */
static Var *find_var(const char *name, int len) {
    for (Var *v = cur_locals; v; v = v->next)
        if ((int)strlen(v->name) == len && strncmp(v->name, name, len) == 0) return v;
    /* L85: also search globals */
    for (Var *v = globals; v; v = v->next)
        if ((int)strlen(v->name) == len && strncmp(v->name, name, len) == 0) return v;
    return NULL;
}

/* Create a new local variable */
static Var *new_lvar(const char *name, int len, Type *ty) {
    Var *v = calloc(1, sizeof(Var));
    snprintf(v->name, 63, "%.*s", len, name);
    v->ty = ty;
    v->next = cur_locals;
    cur_locals = v;
    return v;
}

/*===== Struct type registry =====*/
#define MAX_STRUCTS 32
static struct {
    char name[64];
    Type *ty;
} struct_defs[MAX_STRUCTS];
static int nstruct_defs = 0;

/*===== Enum constant registry =====
 * Enum constants are COMPILE-TIME constants: identifier lookup folds
 * them straight into ND_NUM. (The previous implementation registered
 * them as stack variables whose initializer was never emitted — reads
 * returned uninitialized memory.) */
#define MAX_ENUM_CONSTS 128
static struct {
    char name[64];
    int val;
} enum_consts[MAX_ENUM_CONSTS];
static int nenum_consts = 0;

static int find_enum_const(const char *name, int len, int *out) {
    for (int i = 0; i < nenum_consts; i++)
        if ((int)strlen(enum_consts[i].name) == len && strncmp(enum_consts[i].name, name, len) == 0) {
            *out = enum_consts[i].val;
            return 1;
        }
    return 0;
}

/* Parse 'enum [Name] { A [= n], B, ... };' and register the constants.
 * Used both inside functions and at the top level. */
static Node *parse_enum_decl(void) {
    if (tok->kind == TK_IDENT) tok = tok->next; /* skip optional enum tag */
    skip("{");
    int enum_val = 0;
    while (until("}")) {
        char *ename = tok->loc;
        int elen = tok->len;
        tok = tok->next;
        if (consume("=")) {
            int neg = consume("-");
            enum_val = neg ? -tok->val : tok->val;
            tok = tok->next;
        }
        if (nenum_consts < MAX_ENUM_CONSTS) {
            snprintf(enum_consts[nenum_consts].name, 63, "%.*s", elen, ename);
            enum_consts[nenum_consts].val = enum_val;
            nenum_consts++;
        }
        enum_val++;
        if (!equal("}")) consume(",");
    }
    skip("}");
    skip(";");
    return new_node(ND_BLOCK); /* declarations emit no code */
}

static Type *find_struct(const char *name, int len) {
    for (int i = 0; i < nstruct_defs; i++)
        if ((int)strlen(struct_defs[i].name) == len && strncmp(struct_defs[i].name, name, len) == 0)
            return struct_defs[i].ty;
    return NULL;
}

/* Parse struct definition: struct Name { int x; int y; }; */
static Type *parse_struct_type(void) {
    char *sname = tok->loc;
    int slen = tok->len;
    tok = tok->next;

    /* reference to existing struct (no '{') */
    if (!equal("{")) {
        Type *ty = find_struct(sname, slen);
        if (!ty) {
            fprintf(stderr, "L%d: undefined struct '%.*s'\n", tok->line, slen, sname);
            exit(1);
        }
        return ty;
    }

    /* new struct definition */
    skip("{");
    Type *sty = calloc(1, sizeof(Type));
    sty->kind = TY_STRUCT;
    sty->align = 1;
    Member mhead = {0};
    Member *mcur = &mhead;
    int offset = 0;

    while (until("}")) {
        Type *mty;
        if (equal("int")) {
            mty = ty_int();
            tok = tok->next;
        } else if (equal("char")) {
            mty = ty_char();
            tok = tok->next;
        } else {
            fprintf(stderr, "L%d: expected member type\n", tok->line);
            exit(1);
        }
        while (consume("*")) mty = ptr_to(mty);

        Member *m = calloc(1, sizeof(Member));
        m->ty = mty;
        snprintf(m->name, 63, "%.*s", tok->len, tok->loc);
        tok = tok->next;
        skip(";");
        /* each member starts at an offset aligned to its own alignment
         * (on LP64 a pointer member needs 8-byte alignment) */
        offset = align_to(offset, mty->align);
        m->offset = offset;
        offset += mty->size;
        if (sty->align < mty->align) sty->align = mty->align;
        mcur->next = m;
        mcur = m;
    }
    skip("}");
    sty->members = mhead.next;
    sty->size = align_to(offset, sty->align);

    /* register struct */
    snprintf(struct_defs[nstruct_defs].name, 63, "%.*s", slen, sname);
    struct_defs[nstruct_defs].ty = sty;
    nstruct_defs++;
    return sty;
}

/* Parse a type specifier: int, char, void, struct Name */
static Type *parse_type_spec(void) {
    if (consume("int")) return ty_int();
    if (consume("char")) return ty_char();
    if (consume("void")) return ty_int(); /* treat void as int */
    if (consume("struct")) return parse_struct_type();
    return NULL;
}

/* Parse full type with pointer decorations */
static Type *parse_full_type(void) {
    Type *ty = parse_type_spec();
    if (!ty) return NULL;
    while (consume("*")) ty = ptr_to(ty);
    return ty;
}

/* Check if current token starts a type */
static int at_type(void) { return equal("int") || equal("char") || equal("void") || equal("struct"); }

/*===== Expression parsing (precedence climbing) =====*/
static Node *parse_expr(void);
static Node *parse_assign(void);
static Node *parse_logic_or(void);
static Node *parse_logic_and(void);
static Node *parse_bit_or(void);
static Node *parse_bit_xor(void);
static Node *parse_bit_and(void);
static Node *parse_equality(void);
static Node *parse_relational(void);
static Node *parse_shift(void);
static Node *parse_additive(void);
static Node *parse_multiplicative(void);
static Node *parse_unary(void);
static Node *parse_primary(void);
static Node *parse_compound(void);

static Node *parse_ternary(void);
static Node *new_add(Node *lhs, Node *rhs);
static Node *new_sub(Node *lhs, Node *rhs);

/*===== Compound assignment desugaring =====
 * Naive `A op= B` -> `A = A op B` evaluates the lvalue A TWICE.
 * With side effects inside A this is disastrous:
 *     a[next()] += 1;      // would call next() twice!
 * Standard fix (chibicc-style): capture the ADDRESS of A in a hidden
 * temporary exactly once, then operate through it:
 *     A op= B   =>   (tmp = &A, *tmp = *tmp op B)
 * The hidden temp is a real stack slot with an empty name, so it can
 * never collide with a user variable. Prefix and postfix ++/-- are
 * built on the same helper. */

static Node *new_var_ref(Var *v) {
    Node *n = new_node(ND_VAR);
    n->var = v;
    strcpy(n->name, v->name);
    add_type(n);
    return n;
}

static Node *new_deref(Node *base) {
    Node *n = new_node(ND_DEREF);
    n->lhs = base;
    add_type(n);
    return n;
}

static Node *compound_assign(Node *lhs, int op, Node *rhs) {
    add_type(lhs);
    Var *tmp = new_lvar("", 0, ptr_to(lhs->ty));

    Node *addr = new_node(ND_ADDR);
    addr->lhs = lhs;
    add_type(addr);

    /* tmp = &A */
    Node *capture = new_binary(ND_ASSIGN, new_var_ref(tmp), addr);
    add_type(capture);

    /* *tmp = *tmp op B   (new_add/new_sub keep pointer scaling) */
    Node *inner;
    if (op == ND_ADD)
        inner = new_add(new_deref(new_var_ref(tmp)), rhs);
    else if (op == ND_SUB)
        inner = new_sub(new_deref(new_var_ref(tmp)), rhs);
    else
        inner = new_binary(op, new_deref(new_var_ref(tmp)), rhs);
    Node *apply = new_binary(ND_ASSIGN, new_deref(new_var_ref(tmp)), inner);
    add_type(apply);

    Node *n = new_binary(ND_COMMA, capture, apply);
    add_type(n);
    return n;
}

/* Top-level expression: comma operator (lowest precedence) */
static Node *parse_expr(void) {
    Node *n = parse_assign();
    while (consume(",")) n = new_binary(ND_COMMA, n, parse_assign());
    return n;
}

/* Assignment (right-associative) + compound assignment.
 * Compound forms go through compound_assign so the lvalue is
 * evaluated exactly once (see the desugaring note above). */
static Node *parse_assign(void) {
    Node *n = parse_ternary();
    if (consume("=")) return new_binary(ND_ASSIGN, n, parse_assign());
    if (consume("+=")) return compound_assign(n, ND_ADD, parse_assign());
    if (consume("-=")) return compound_assign(n, ND_SUB, parse_assign());
    if (consume("*=")) return compound_assign(n, ND_MUL, parse_assign());
    if (consume("/=")) return compound_assign(n, ND_DIV, parse_assign());
    if (consume("%=")) return compound_assign(n, ND_MOD, parse_assign());
    if (consume("&=")) return compound_assign(n, ND_BITAND, parse_assign());
    if (consume("|=")) return compound_assign(n, ND_BITOR, parse_assign());
    if (consume("^=")) return compound_assign(n, ND_BITXOR, parse_assign());
    if (consume("<<=")) return compound_assign(n, ND_SHL, parse_assign());
    if (consume(">>=")) return compound_assign(n, ND_SHR, parse_assign());
    return n;
}

/* Ternary conditional: cond ? true_expr : false_expr */
static Node *parse_ternary(void) {
    Node *cond = parse_logic_or();
    if (!consume("?")) return cond;
    Node *n = new_node(ND_COND);
    n->cond = cond;
    n->cond_true = parse_expr();
    skip(":");
    n->cond_false = parse_ternary();
    return n;
}

static Node *parse_logic_or(void) {
    Node *n = parse_logic_and();
    while (consume("||")) n = new_binary(ND_OR, n, parse_logic_and());
    return n;
}

static Node *parse_logic_and(void) {
    Node *n = parse_bit_or();
    while (consume("&&")) n = new_binary(ND_AND, n, parse_bit_or());
    return n;
}

static Node *parse_bit_or(void) {
    Node *n = parse_bit_xor();
    while (consume("|")) n = new_binary(ND_BITOR, n, parse_bit_xor());
    return n;
}

static Node *parse_bit_xor(void) {
    Node *n = parse_bit_and();
    while (consume("^")) n = new_binary(ND_BITXOR, n, parse_bit_and());
    return n;
}

static Node *parse_bit_and(void) {
    Node *n = parse_equality();
    /* Note: '&' as bitwise AND must not be confused with unary '&' (address-of)
     * Here we only see '&' as binary if it appears after an expression. */
    while (consume("&")) n = new_binary(ND_BITAND, n, parse_equality());
    return n;
}

static Node *parse_equality(void) {
    Node *n = parse_relational();
    while (1) {
        if (consume("==")) {
            n = new_binary(ND_EQ, n, parse_relational());
            continue;
        }
        if (consume("!=")) {
            n = new_binary(ND_NE, n, parse_relational());
            continue;
        }
        break;
    }
    return n;
}

static Node *parse_relational(void) {
    Node *n = parse_shift();
    while (1) {
        if (consume("<=")) {
            n = new_binary(ND_LE, n, parse_shift());
            continue;
        }
        if (consume(">=")) {
            n = new_binary(ND_GE, n, parse_shift());
            continue;
        }
        if (consume("<")) {
            n = new_binary(ND_LT, n, parse_shift());
            continue;
        }
        if (consume(">")) {
            n = new_binary(ND_GT, n, parse_shift());
            continue;
        }
        break;
    }
    return n;
}

static Node *parse_shift(void) {
    Node *n = parse_additive();
    while (1) {
        if (consume("<<")) {
            n = new_binary(ND_SHL, n, parse_additive());
            continue;
        }
        if (consume(">>")) {
            n = new_binary(ND_SHR, n, parse_additive());
            continue;
        }
        break;
    }
    return n;
}

/* Type-aware add (chibicc-style): annotate operands eagerly, then
 * scale the integer side when the other side is a pointer/array.
 * Works for ANY subexpression — (p+1)+1, f()+i, a[i]+j — not just
 * direct variable references. */
static Node *new_add(Node *lhs, Node *rhs) {
    add_type(lhs);
    add_type(rhs);
    Type *lt = decay_array(lhs->ty);
    Type *rt = decay_array(rhs->ty);

    /* int + ptr → canonicalize to ptr + int */
    if (!is_pointer_like(lt) && is_pointer_like(rt)) {
        Node *tn = lhs;
        lhs = rhs;
        rhs = tn;
        Type *tt = lt;
        lt = rt;
        rt = tt;
    }

    /* ptr + int: scale the integer by element size */
    if (is_pointer_like(lt) && !is_pointer_like(rt)) {
        int esz = (lt->base) ? lt->base->size : 1;
        if (esz > 1) rhs = new_binary(ND_MUL, rhs, new_num(esz));
    }

    Node *n = new_binary(ND_ADD, lhs, rhs);
    add_type(n);
    return n;
}

/* Type-aware sub: ptr - int scales, ptr - ptr yields element count */
static Node *new_sub(Node *lhs, Node *rhs) {
    add_type(lhs);
    add_type(rhs);
    Type *lt = decay_array(lhs->ty);
    Type *rt = decay_array(rhs->ty);

    /* ptr - ptr → (byte difference) / element size */
    if (is_pointer_like(lt) && is_pointer_like(rt)) {
        int esz = (lt->base) ? lt->base->size : 1;
        Node *n = new_binary(ND_SUB, lhs, rhs);
        n->ty = ty_int();
        if (esz > 1) {
            n = new_binary(ND_DIV, n, new_num(esz));
            n->ty = ty_int();
        }
        return n;
    }

    /* ptr - int: scale the integer by element size */
    if (is_pointer_like(lt)) {
        int esz = (lt->base) ? lt->base->size : 1;
        if (esz > 1) rhs = new_binary(ND_MUL, rhs, new_num(esz));
    }

    Node *n = new_binary(ND_SUB, lhs, rhs);
    add_type(n);
    return n;
}

static Node *parse_additive(void) {
    Node *n = parse_multiplicative();
    while (1) {
        if (consume("+")) {
            n = new_add(n, parse_multiplicative());
            continue;
        }
        if (consume("-")) {
            n = new_sub(n, parse_multiplicative());
            continue;
        }
        break;
    }
    return n;
}

static Node *parse_multiplicative(void) {
    Node *n = parse_unary();
    while (1) {
        if (consume("*")) {
            n = new_binary(ND_MUL, n, parse_unary());
            continue;
        }
        if (consume("/")) {
            n = new_binary(ND_DIV, n, parse_unary());
            continue;
        }
        if (consume("%")) {
            n = new_binary(ND_MOD, n, parse_unary());
            continue;
        }
        break;
    }
    return n;
}

static Node *parse_postfix(void);

/* Check if current position is '(' followed by a type keyword — indicates cast */
static int is_cast(void) {
    if (!equal("(")) return 0;
    Token *t = tok->next;
    if (!t) return 0;
    int tlen = t->len;
    char *tloc = t->loc;
    if ((tlen == 3 && strncmp(tloc, "int", 3) == 0) || (tlen == 4 && strncmp(tloc, "char", 4) == 0) ||
        (tlen == 4 && strncmp(tloc, "void", 4) == 0) || (tlen == 6 && strncmp(tloc, "struct", 6) == 0))
        return 1;
    return 0;
}

static Node *parse_unary(void) {
    if (consume("-")) return new_binary(ND_NEG, parse_unary(), NULL);
    if (consume("!")) return new_binary(ND_NOT, parse_unary(), NULL);
    if (consume("~")) return new_binary(ND_BITNOT, parse_unary(), NULL);
    /* L88: address-of and dereference */
    if (consume("&")) {
        Node *n = new_node(ND_ADDR);
        n->lhs = parse_unary();
        return n;
    }
    if (consume("*")) {
        Node *n = new_node(ND_DEREF);
        n->lhs = parse_unary();
        return n;
    }
    /* prefix ++/-- : value is the NEW value; lvalue evaluated once */
    if (consume("++")) {
        Node *operand = parse_unary();
        return compound_assign(operand, ND_ADD, new_num(1));
    }
    if (consume("--")) {
        Node *operand = parse_unary();
        return compound_assign(operand, ND_SUB, new_num(1));
    }
    /* sizeof — fully type-driven: sizeof(type) parses the type
     * (pointers included, width = target_ptr_size), sizeof(expr)
     * annotates the expression and reads its type. Arrays report
     * their full size, matching C semantics. */
    if (consume("sizeof")) {
        skip("(");
        int sz = 4;
        if (at_type()) {
            Type *ty = parse_full_type();
            if (ty) sz = ty->size;
        } else {
            Node *e = parse_expr();
            add_type(e);
            if (e->ty) sz = e->ty->size;
        }
        skip(")");
        return new_num(sz);
    }
    /* cast expression: (type)expr */
    if (is_cast()) {
        skip("(");
        Type *cast_ty = parse_full_type();
        skip(")");
        Node *n = new_node(ND_CAST);
        n->lhs = parse_unary();
        n->ty = cast_ty; /* the node's type IS the cast target */
        return n;
    }
    return parse_postfix();
}

/* Postfix operators: a[i], a.x, a++, a-- */
static Node *parse_postfix(void) {
    Node *n = parse_primary();
    while (1) {
        /* array subscript a[i] -> *(a + i), scaling handled by new_add */
        if (consume("[")) {
            Node *idx = parse_expr();
            skip("]");
            n = new_add(n, idx);
            Node *deref = new_node(ND_DEREF);
            deref->lhs = n;
            add_type(deref);
            n = deref;
            continue;
        }
        /* struct member a.x */
        if (consume(".")) {
            Node *m = new_node(ND_MEMBER);
            m->lhs = n;
            snprintf(m->name, 63, "%.*s", tok->len, tok->loc);
            tok = tok->next;
            n = m;
            continue;
        }
        /* arrow operator p->x  =>  (*p).x */
        if (consume("->")) {
            Node *deref = new_node(ND_DEREF);
            deref->lhs = n;
            Node *m = new_node(ND_MEMBER);
            m->lhs = deref;
            snprintf(m->name, 63, "%.*s", tok->len, tok->loc);
            tok = tok->next;
            n = m;
            continue;
        }
        /* postfix ++/-- yields the OLD value:
         *   a++  =>  (tmp = &a, *tmp = *tmp + 1) - 1
         * compound_assign captures the address once (side effects in
         * the lvalue run once); the trailing -1/+1 recovers the old
         * value, pointer-aware via new_sub/new_add. */
        if (consume("++")) {
            n = new_sub(compound_assign(n, ND_ADD, new_num(1)), new_num(1));
            add_type(n);
            continue;
        }
        if (consume("--")) {
            n = new_add(compound_assign(n, ND_SUB, new_num(1)), new_num(1));
            add_type(n);
            continue;
        }
        break;
    }
    return n;
}

static Node *parse_primary(void) {
    if (consume("(")) {
        Node *n = parse_expr();
        skip(")");
        return n;
    }

    if (tok->kind == TK_NUM) {
        Node *n = new_num(tok->val);
        tok = tok->next;
        return n;
    }

    /* L86: string literal */
    if (tok->kind == TK_STRING) {
        Node *n = new_node(ND_STR);
        n->str_val = malloc(tok->len + 1);
        memcpy(n->str_val, tok->loc, tok->len);
        n->str_val[tok->len] = '\0';
        n->str_id = str_count++;
        tok = tok->next;
        return n;
    }

    /* L77/L82: identifier (variable or function call) */
    if (tok->kind == TK_IDENT) {
        char *name = tok->loc;
        int len = tok->len;
        tok = tok->next;

        /* L82: function call: name(...)
         * Each argument is an ASSIGNMENT expression - parse_expr would
         * swallow the separating commas as comma operators and collapse
         * every multi-argument call into a single argument. */
        if (consume("(")) {
            Node *call = new_node(ND_CALL);
            snprintf(call->name, 63, "%.*s", len, name);
            Node head = {0};
            Node *cur = &head;
            int nargs = 0;
            if (!equal(")")) {
                cur->next = parse_assign();
                cur = cur->next;
                nargs++;
                while (consume(",")) {
                    cur->next = parse_assign();
                    cur = cur->next;
                    nargs++;
                }
            }
            skip(")");
            call->args = head.next;
            call->nargs = nargs;
            return call;
        }

        /* enum constant: fold to a literal at parse time */
        int ev;
        if (find_enum_const(name, len, &ev)) return new_num(ev);

        /* L77: variable reference */
        Var *v = find_var(name, len);
        if (!v) {
            /* try forward-reference: create implicit int variable */
            fprintf(stderr, "\033[1;33mwarn\033[0m at L%d: implicit declaration of '%.*s'\n", tok->line, len, name);
            v = new_lvar(name, len, ty_int());
        }
        Node *n = new_node(ND_VAR);
        snprintf(n->name, 63, "%.*s", len, name);
        n->var = v;
        return n; /* postfix [] and . are handled by parse_postfix */
    }

    /* error recovery: report and skip to next statement boundary */
    error_token(tok, "expected expression");
    skip_to_recovery();
    return new_num(0); /* return dummy value to continue parsing */
}

/*===== Statement parsing =====*/
static Node *parse_stmt(void) {
    /* return statement */
    if (consume("return")) {
        Node *n = new_node(ND_RETURN);
        if (!equal(";")) n->lhs = parse_expr();
        skip(";");
        return n;
    }

    /* L79: if statement */
    if (consume("if")) {
        Node *n = new_node(ND_IF);
        skip("(");
        n->cond = parse_expr();
        skip(")");
        n->then = parse_stmt();
        if (consume("else")) n->els = parse_stmt();
        return n;
    }

    /* L80: while statement */
    if (consume("while")) {
        Node *n = new_node(ND_WHILE);
        skip("(");
        n->cond = parse_expr();
        skip(")");
        n->body = parse_stmt();
        return n;
    }

    /* L80: for statement */
    if (consume("for")) {
        Node *n = new_node(ND_FOR);
        skip("(");
        /* init: can be declaration or expression */
        if (at_type()) {
            Type *fty = parse_full_type();
            char *vname = tok->loc;
            int vlen = tok->len;
            tok = tok->next;
            Var *v = new_lvar(vname, vlen, fty);
            Node *decl = new_node(ND_VAR_DECL);
            decl->var = v;
            snprintf(decl->name, 63, "%.*s", vlen, vname);
            if (consume("=")) decl->lhs = parse_expr();
            skip(";");
            n->init = decl;
        } else {
            /* expression init must be wrapped as a STATEMENT — codegen's
             * gen_stmt(node->init) only accepts statement kinds */
            if (!equal(";")) {
                Node *e = new_node(ND_EXPR_STMT);
                e->lhs = parse_expr();
                n->init = e;
            }
            skip(";");
        }
        if (!equal(";")) n->cond = parse_expr();
        skip(";");
        if (!equal(")")) n->inc = parse_expr();
        skip(")");
        n->body = parse_stmt();
        return n;
    }

    /* L81: do-while statement */
    if (consume("do")) {
        Node *n = new_node(ND_DOWHILE);
        n->body = parse_stmt();
        skip("while");
        skip("(");
        n->cond = parse_expr();
        skip(")");
        skip(";");
        return n;
    }

    /* L81: break */
    if (consume("break")) {
        skip(";");
        return new_node(ND_BREAK);
    }

    /* L81: continue */
    if (consume("continue")) {
        skip(";");
        return new_node(ND_CONTINUE);
    }

    /* switch statement */
    if (consume("switch")) {
        Node *n = new_node(ND_SWITCH);
        skip("(");
        n->cond = parse_expr();
        skip(")");
        skip("{");
        Node case_head = {0};
        Node *case_cur = &case_head;
        Node body_head = {0};
        Node *body_cur = &body_head;
        while (until("}")) {
            if (consume("case")) {
                Node *c = new_node(ND_CASE);
                /* case label: integer literal, -literal, or enum constant */
                int neg = consume("-");
                int cv = 0;
                if (tok->kind == TK_NUM)
                    cv = tok->val;
                else if (!find_enum_const(tok->loc, tok->len, &cv))
                    error_token(tok, "case label must be a constant");
                c->case_val = neg ? -cv : cv;
                tok = tok->next;
                skip(":");
                case_cur->next = c;
                case_cur = c;
                /* parse statements until next case/default/} */
                Node stmt_head = {0};
                Node *stmt_cur = &stmt_head;
                while (tok->kind != TK_EOF && !equal("case") && !equal("default") && !equal("}")) {
                    stmt_cur->next = parse_stmt();
                    stmt_cur = stmt_cur->next;
                }
                c->body = stmt_head.next;
            } else if (consume("default")) {
                skip(":");
                Node *c = new_node(ND_DEFAULT);
                case_cur->next = c;
                case_cur = c;
                Node stmt_head = {0};
                Node *stmt_cur = &stmt_head;
                while (tok->kind != TK_EOF && !equal("case") && !equal("}")) {
                    stmt_cur->next = parse_stmt();
                    stmt_cur = stmt_cur->next;
                }
                c->body = stmt_head.next;
            } else {
                body_cur->next = parse_stmt();
                body_cur = body_cur->next;
            }
        }
        skip("}");
        n->cases = case_head.next;
        n->body = body_head.next;
        return n;
    }

    /* enum declaration: registers compile-time constants */
    if (consume("enum")) return parse_enum_decl();

    /* L77: compound statement (block) */
    if (equal("{")) return parse_compound();

    /* L77/L86/L87/L88/L91: variable declaration */
    if (at_type()) {
        Type *ty = parse_full_type();
        char *name = tok->loc;
        int len = tok->len;
        tok = tok->next; /* skip name */

        /* L87: array declaration: int a[N] or int a[] */
        if (consume("[")) {
            int arr_len = 0;
            if (tok->kind == TK_NUM) {
                arr_len = tok->val;
                tok = tok->next;
            }
            skip("]");

            /* optional brace initializer: = {expr, expr, ...} */
            if (consume("=") && equal("{")) {
                skip("{");
                /* count and parse initializer elements */
                Node init_head = {0};
                Node *init_cur = &init_head;
                int count = 0;
                while (until("}")) {
                    init_cur->next = parse_assign();
                    init_cur = init_cur->next;
                    count++;
                    if (!equal("}")) skip(",");
                }
                skip("}");

                /* deduce array length from initializer if a[] */
                if (arr_len == 0) arr_len = count;
                ty = array_of(ty, arr_len);

                Var *v = new_lvar(name, len, ty);
                /* generate a block: { a[0]=e0; a[1]=e1; ... } */
                Node *block = new_node(ND_BLOCK);
                Node body_head = {0};
                Node *body_cur = &body_head;

                /* first: the var_decl itself (no init expression) */
                Node *decl = new_node(ND_VAR_DECL);
                decl->var = v;
                snprintf(decl->name, 63, "%.*s", len, name);
                body_cur->next = decl;
                body_cur = body_cur->next;

                /* then: a[i] = expr for each element
                 * (element scaling handled by type-aware new_add) */
                int idx = 0;
                for (Node *e = init_head.next; e && idx < arr_len; e = e->next, idx++) {
                    Node *var_ref = new_node(ND_VAR);
                    snprintf(var_ref->name, 63, "%.*s", len, name);
                    var_ref->var = v;
                    Node *addr = new_add(var_ref, new_num(idx));
                    Node *deref = new_node(ND_DEREF);
                    deref->lhs = addr;
                    add_type(deref);
                    Node *assign = new_binary(ND_ASSIGN, deref, e);
                    Node *stmt = new_node(ND_EXPR_STMT);
                    stmt->lhs = assign;
                    body_cur->next = stmt;
                    body_cur = body_cur->next;
                }
                block->body = body_head.next;
                skip(";");
                return block;
            } else {
                /* no initializer or non-brace init */
                if (arr_len == 0) arr_len = 1; /* fallback */
                ty = array_of(ty, arr_len);
            }

            Var *v = new_lvar(name, len, ty);
            Node *n = new_node(ND_VAR_DECL);
            n->var = v;
            snprintf(n->name, 63, "%.*s", len, name);
            skip(";");
            return n;
        }

        /* multi-variable declaration: int a=1, b=2, *c; */
        Node *block = new_node(ND_BLOCK);
        Node body_head = {0};
        Node *body_cur = &body_head;

        /* first declarator */
        Type *vty = ty;
        while (consume("*")) vty = ptr_to(vty);
        Var *v = new_lvar(name, len, vty);
        Node *decl = new_node(ND_VAR_DECL);
        decl->var = v;
        snprintf(decl->name, 63, "%.*s", len, name);
        if (consume("=")) decl->lhs = parse_assign();
        body_cur->next = decl;
        body_cur = body_cur->next;

        /* additional declarators separated by ',' */
        while (consume(",")) {
            Type *next_ty = ty;
            while (consume("*")) next_ty = ptr_to(next_ty);
            char *vname = tok->loc;
            int vlen = tok->len;
            tok = tok->next;
            /* check for array suffix */
            if (consume("[")) {
                int alen = tok->val;
                tok = tok->next;
                skip("]");
                next_ty = array_of(next_ty, alen);
            }
            Var *nv = new_lvar(vname, vlen, next_ty);
            Node *nd = new_node(ND_VAR_DECL);
            nd->var = nv;
            snprintf(nd->name, 63, "%.*s", vlen, vname);
            if (consume("=")) nd->lhs = parse_assign();
            body_cur->next = nd;
            body_cur = body_cur->next;
        }
        skip(";");

        /* if only one declarator, return it directly (no block wrapper) */
        if (body_head.next && !body_head.next->next) return body_head.next;
        block->body = body_head.next;
        return block;
    }

    /* L78: expression statement */
    Node *n = new_node(ND_EXPR_STMT);
    n->lhs = parse_expr();
    skip(";");
    return n;
}

/* L77: compound statement { stmt* } */
static Node *parse_compound(void) {
    skip("{");
    Node *block = new_node(ND_BLOCK);
    Node head = {0};
    Node *cur = &head;
    while (until("}")) {
        cur->next = parse_stmt();
        cur = cur->next;
    }
    skip("}");
    block->body = head.next;
    return block;
}

/* L85: parse global variable: type name [= val]; or type name[N]; */
static Node *parse_global_var(const char *name, int len, Type *ty) {
    Var *v = calloc(1, sizeof(Var));
    snprintf(v->name, 63, "%.*s", len, name);
    v->is_global = 1;

    /* handle global array: name[N] */
    if (consume("[")) {
        int arr_len = 0;
        if (tok->kind == TK_NUM) {
            arr_len = tok->val;
            tok = tok->next;
        }
        skip("]");
        if (arr_len == 0) arr_len = 1;
        ty = array_of(ty, arr_len);
    }

    v->ty = ty;
    if (consume("=")) {
        int neg = consume("-"); /* allow 'int g = -5;' */
        v->init_val = neg ? -tok->val : tok->val;
        tok = tok->next;
    }
    skip(";");
    v->next = globals;
    globals = v;

    Node *n = new_node(ND_VAR_DECL);
    n->var = v;
    strcpy(n->name, v->name);
    return n;
}

Node *parse(Token *tokens) {
    tok = tokens;
    globals = NULL;
    str_count = 0;

    Node *prog = new_node(ND_PROGRAM);
    Node head = {0};
    Node *cur = &head;

    nstruct_defs = 0;
    nenum_consts = 0;

    while (tok->kind != TK_EOF) {
        /* top-level enum declaration */
        if (consume("enum")) {
            parse_enum_decl();
            continue;
        }

        /* parse type (int/char/void/struct) */
        Type *base_ty = parse_full_type();
        if (!base_ty) {
            if (consume(";")) continue;
            fprintf(stderr, "L%d: expected type at top level\n", tok->line);
            exit(1);
        }

        /* bare struct definition at top level: struct Name { ... }; */
        if (consume(";")) continue;

        char *name = tok->loc;
        int len = tok->len;
        tok = tok->next;

        if (equal("(")) {
            /* function definition */
            Node *fn = new_node(ND_FUNC);
            cur_locals = NULL;
            snprintf(fn->name, 63, "%.*s", len, name);

            skip("(");
            Node phead = {0};
            Node *pcur = &phead;
            int np = 0;
            if (!equal(")") && !equal("void")) {
                do {
                    Type *pty = parse_full_type();
                    if (!pty) pty = ty_int();
                    Var *pv = new_lvar(tok->loc, tok->len, pty);
                    Node *p = new_node(ND_VAR);
                    snprintf(p->name, 63, "%.*s", tok->len, tok->loc);
                    p->var = pv;
                    tok = tok->next;
                    pcur->next = p;
                    pcur = p;
                    np++;
                } while (consume(","));
            }
            if (equal("void")) tok = tok->next;
            skip(")");
            fn->params = phead.next;
            fn->nparams = np;

            fn->body = parse_compound();

            /* assign stack offsets: each slot [fp-offset, fp-offset+size)
             * must start at an address aligned to the variable's own
             * alignment (8 for pointers on LP64). fp itself is 16-aligned,
             * so aligning the offset is sufficient. */
            int offset = 0;
            for (Var *v = cur_locals; v; v = v->next) {
                int sz = v->ty ? v->ty->size : 4;
                int al = (v->ty && v->ty->align) ? v->ty->align : 4;
                offset += sz;
                offset = align_to(offset, al);
                v->offset = offset;
            }
            fn->locals = cur_locals;
            fn->stack_size = align_to(offset, 16);

            cur->next = fn;
            cur = cur->next;
        } else {
            /* global variable */
            cur->next = parse_global_var(name, len, base_ty);
            cur = cur->next;
        }
    }

    /* error recovery lets us report multiple errors, but a program that
     * failed to parse must never reach codegen (or exit with status 0) */
    if (parse_errors > 0) {
        fprintf(stderr, "%d error%s generated\n", parse_errors, parse_errors > 1 ? "s" : "");
        exit(1);
    }

    prog->body = head.next;
    return prog;
}
