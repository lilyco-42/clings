/*
 * type.c - Type system for nccl-cc
 *
 * Lesson 87+: int, char, pointer, array, struct types
 * Type compatibility, size calculation, pointer arithmetic helpers
 */
#include "nccl_cc.h"

/* L95: pointer size of the TARGET architecture (not the host).
 * RV32/ARM32 use the ILP32 data model (pointer = 4 bytes),
 * x86-64 uses LP64 (pointer = 8 bytes).
 * main() must set this before parsing: sizeof, struct layout and
 * stack offsets are all computed at parse time. */
int target_ptr_size = 4;

static Type int_type_inst = { TY_INT, 4, 4, NULL, 0, NULL };
static Type char_type_inst = { TY_CHAR, 1, 1, NULL, 0, NULL };

Type *ty_int(void) { return &int_type_inst; }
Type *ty_char(void) { return &char_type_inst; }

/* Round n up to the next multiple of align (align must be a power of 2) */
int align_to(int n, int align)
{
    return (n + align - 1) & ~(align - 1);
}

Type *ptr_to(Type *base)
{
    Type *ty = calloc(1, sizeof(Type));
    ty->kind = TY_PTR;
    ty->size = target_ptr_size;
    ty->align = target_ptr_size;
    ty->base = base;
    return ty;
}

Type *array_of(Type *base, int len)
{
    Type *ty = calloc(1, sizeof(Type));
    ty->kind = TY_ARRAY;
    ty->size = base->size * len;
    ty->align = base->align;
    ty->base = base;
    ty->array_len = len;
    return ty;
}

/* Get the element size for pointer arithmetic:
 * ptr + 1 should advance by sizeof(*ptr) bytes */
int ptr_elem_size(Type *ty)
{
    if (!ty) return 4;
    if (ty->kind == TY_PTR && ty->base)
        return ty->base->size;
    if (ty->kind == TY_ARRAY && ty->base)
        return ty->base->size;
    return 4;
}

/* Check if type is a pointer or array (decays to pointer) */
int is_pointer_like(Type *ty)
{
    if (!ty) return 0;
    return ty->kind == TY_PTR || ty->kind == TY_ARRAY;
}

/* Check if two types are compatible for assignment */
int types_compatible(Type *a, Type *b)
{
    if (!a || !b) return 1; /* lenient: unknown types are compatible */
    if (a->kind == b->kind) return 1;
    /* int and char are compatible */
    if ((a->kind == TY_INT || a->kind == TY_CHAR) &&
        (b->kind == TY_INT || b->kind == TY_CHAR))
        return 1;
    /* pointer types: any pointer assigns to any pointer */
    if (is_pointer_like(a) && is_pointer_like(b))
        return 1;
    /* int to pointer (e.g. NULL) */
    if ((a->kind == TY_INT && is_pointer_like(b)) ||
        (is_pointer_like(a) && b->kind == TY_INT))
        return 1;
    return 0;
}

/* Get human-readable type name for error messages */
const char *type_name(Type *ty)
{
    if (!ty) return "unknown";
    switch (ty->kind) {
    case TY_INT:    return "int";
    case TY_CHAR:   return "char";
    case TY_PTR:    return "pointer";
    case TY_ARRAY:  return "array";
    case TY_STRUCT: return "struct";
    default:        return "unknown";
    }
}

/* Find a member by name in a struct type */
Member *find_member(Type *ty, const char *name)
{
    if (!ty || ty->kind != TY_STRUCT) return NULL;
    for (Member *m = ty->members; m; m = m->next)
        if (strcmp(m->name, name) == 0)
            return m;
    return NULL;
}

/* Array-to-pointer decay: array of T → pointer to T
 * Returns the decayed type, or the original type if not an array. */
Type *decay_array(Type *ty)
{
    if (!ty) return ty;
    if (ty->kind == TY_ARRAY)
        return ptr_to(ty->base);
    return ty;
}

/* Get sizeof for a type (handles all type kinds) */
int type_sizeof(Type *ty)
{
    if (!ty) return 4;
    return ty->size;
}

/* Infer the result type of a binary addition involving pointers.
 * ptr + int → ptr, int + ptr → ptr, int + int → int */
Type *add_result_type(Type *lhs, Type *rhs)
{
    /* decay arrays first */
    lhs = decay_array(lhs);
    rhs = decay_array(rhs);

    if (!lhs || !rhs) return ty_int();

    /* ptr + int or array + int → ptr */
    if (is_pointer_like(lhs) && (rhs->kind == TY_INT || rhs->kind == TY_CHAR))
        return lhs;
    /* int + ptr → ptr */
    if ((lhs->kind == TY_INT || lhs->kind == TY_CHAR) && is_pointer_like(rhs))
        return rhs;
    /* int + int → int */
    return ty_int();
}

/* Infer the result type of a binary subtraction involving pointers.
 * ptr - int → ptr, ptr - ptr → int (distance), int - int → int */
Type *sub_result_type(Type *lhs, Type *rhs)
{
    lhs = decay_array(lhs);
    rhs = decay_array(rhs);

    if (!lhs || !rhs) return ty_int();

    /* ptr - int → ptr */
    if (is_pointer_like(lhs) && (rhs->kind == TY_INT || rhs->kind == TY_CHAR))
        return lhs;
    /* ptr - ptr → int (number of elements) */
    if (is_pointer_like(lhs) && is_pointer_like(rhs))
        return ty_int();
    return ty_int();
}

/* Check if a type is an integer type (int or char) */
int is_integer_type(Type *ty)
{
    if (!ty) return 1; /* lenient */
    return ty->kind == TY_INT || ty->kind == TY_CHAR;
}

/* Promote char to int for arithmetic operations */
Type *promote_type(Type *ty)
{
    if (!ty) return ty_int();
    if (ty->kind == TY_CHAR)
        return ty_int();
    return ty;
}

/* Get the common type for binary operations between two types */
Type *common_type(Type *a, Type *b)
{
    a = promote_type(a);
    b = promote_type(b);
    if (!a) return b;
    if (!b) return a;
    /* if either is a pointer, prefer pointer */
    if (is_pointer_like(a)) return a;
    if (is_pointer_like(b)) return b;
    return ty_int();
}

/*===== L95: type annotation pass (chibicc-style) =====
 *
 * Walks the AST bottom-up and attaches a Type to every expression node.
 * Codegen then selects load/store widths from node->ty->size instead of
 * guessing from node->var — this is what makes *p, **pp, p->x and char*
 * writes work on every backend, and what makes the LP64 (x86-64) backend
 * possible at all.
 *
 * Re-visiting a shared subtree (compound-assignment desugaring shares
 * nodes) is harmless: the AST is a DAG, recursion terminates, and an
 * already-annotated node keeps its type. */
void add_type(Node *node)
{
    if (!node) return;

    add_type(node->lhs);
    add_type(node->rhs);
    add_type(node->cond);
    add_type(node->then);
    add_type(node->els);
    add_type(node->init);
    add_type(node->inc);
    add_type(node->cond_true);
    add_type(node->cond_false);

    for (Node *n = node->body; n; n = n->next) add_type(n);
    for (Node *n = node->args; n; n = n->next) add_type(n);
    for (Node *n = node->cases; n; n = n->next) add_type(n);
    for (Node *n = node->params; n; n = n->next) add_type(n);

    if (node->ty) return; /* already annotated (e.g. ND_CAST set by parser) */

    switch (node->kind) {
    case ND_NUM:
        node->ty = ty_int();
        break;
    case ND_VAR:
    case ND_VAR_DECL:
        node->ty = (node->var && node->var->ty) ? node->var->ty : ty_int();
        break;
    case ND_STR:
        node->ty = ptr_to(ty_char());
        break;
    case ND_ADD:
        node->ty = add_result_type(node->lhs->ty, node->rhs->ty);
        break;
    case ND_SUB:
        node->ty = sub_result_type(node->lhs->ty, node->rhs->ty);
        break;
    case ND_ASSIGN:
        node->ty = node->lhs->ty;
        break;
    case ND_ADDR:
        /* &array decays to pointer-to-element (good enough for our subset) */
        if (node->lhs->ty && node->lhs->ty->kind == TY_ARRAY)
            node->ty = ptr_to(node->lhs->ty->base);
        else
            node->ty = ptr_to(node->lhs->ty ? node->lhs->ty : ty_int());
        break;
    case ND_DEREF:
        if (node->lhs->ty && node->lhs->ty->base)
            node->ty = node->lhs->ty->base;
        else
            node->ty = ty_int(); /* *int — tolerated, reads 'int' width */
        break;
    case ND_MEMBER: {
        Type *t = node->lhs->ty;
        Member *m = t ? find_member(t, node->name) : NULL;
        if (m) {
            node->member = m;
            node->ty = m->ty;
        } else {
            node->ty = ty_int();
        }
        break;
    }
    case ND_COMMA:
        node->ty = node->rhs->ty;
        break;
    case ND_COND:
        node->ty = node->cond_true ? node->cond_true->ty : ty_int();
        break;
    case ND_CALL:
        node->ty = ty_int(); /* no prototypes: every function returns int */
        break;
    case ND_CAST:
        node->ty = (node->var && node->var->ty) ? node->var->ty : ty_int();
        break;
    case ND_NEG: case ND_NOT: case ND_BITNOT:
    case ND_MUL: case ND_DIV: case ND_MOD:
    case ND_EQ: case ND_NE: case ND_LT: case ND_LE: case ND_GT: case ND_GE:
    case ND_AND: case ND_OR:
    case ND_BITAND: case ND_BITOR: case ND_BITXOR:
    case ND_SHL: case ND_SHR:
        node->ty = ty_int();
        break;
    default:
        /* statements (if/while/block/...) carry no type */
        break;
    }
}
