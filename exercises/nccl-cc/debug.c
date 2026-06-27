/*
 * debug.c - developer tools for nccl-cc
 *
 * -dump-tokens : print the token stream after preprocessing/lexing
 * -dump-ast    : print the annotated AST (with types) after parsing
 *
 * Both are invaluable when a test fails: they show exactly what the
 * compiler SAW (tokens) and what it UNDERSTOOD (tree + types).
 */
#include "nccl_cc.h"

/*===== token dump =====*/

static const char *tk_names[] = {"NUM", "IDENT", "KEYWORD", "PUNCT", "STRING", "EOF"};

void dump_tokens(Token *tok) {
    for (Token *t = tok; t; t = t->next) {
        printf("L%-4d %-8s", t->line, tk_names[t->kind]);
        if (t->kind == TK_NUM) {
            printf(" %d", t->val);
        } else if (t->kind == TK_STRING) {
            printf(" ");
            emit_escaped_string(t->loc);
        } else if (t->kind != TK_EOF) {
            printf(" '%.*s'", t->len, t->loc);
        }
        printf("\n");
        if (t->kind == TK_EOF) break;
    }
}

/*===== AST dump =====*/

static const char *nd_name(int kind) {
    switch (kind) {
        case ND_NUM:
            return "NUM";
        case ND_VAR:
            return "VAR";
        case ND_ASSIGN:
            return "ASSIGN";
        case ND_NEG:
            return "NEG";
        case ND_NOT:
            return "NOT";
        case ND_BITNOT:
            return "BITNOT";
        case ND_ADD:
            return "ADD";
        case ND_SUB:
            return "SUB";
        case ND_MUL:
            return "MUL";
        case ND_DIV:
            return "DIV";
        case ND_MOD:
            return "MOD";
        case ND_EQ:
            return "EQ";
        case ND_NE:
            return "NE";
        case ND_LT:
            return "LT";
        case ND_LE:
            return "LE";
        case ND_GT:
            return "GT";
        case ND_GE:
            return "GE";
        case ND_AND:
            return "AND";
        case ND_OR:
            return "OR";
        case ND_BITAND:
            return "BITAND";
        case ND_BITOR:
            return "BITOR";
        case ND_BITXOR:
            return "BITXOR";
        case ND_SHL:
            return "SHL";
        case ND_SHR:
            return "SHR";
        case ND_RETURN:
            return "RETURN";
        case ND_IF:
            return "IF";
        case ND_WHILE:
            return "WHILE";
        case ND_FOR:
            return "FOR";
        case ND_DOWHILE:
            return "DOWHILE";
        case ND_BREAK:
            return "BREAK";
        case ND_CONTINUE:
            return "CONTINUE";
        case ND_EXPR_STMT:
            return "EXPR_STMT";
        case ND_VAR_DECL:
            return "VAR_DECL";
        case ND_BLOCK:
            return "BLOCK";
        case ND_ADDR:
            return "ADDR";
        case ND_DEREF:
            return "DEREF";
        case ND_STR:
            return "STR";
        case ND_MEMBER:
            return "MEMBER";
        case ND_COND:
            return "COND";
        case ND_COMMA:
            return "COMMA";
        case ND_CAST:
            return "CAST";
        case ND_SWITCH:
            return "SWITCH";
        case ND_CASE:
            return "CASE";
        case ND_DEFAULT:
            return "DEFAULT";
        case ND_CALL:
            return "CALL";
        case ND_FUNC:
            return "FUNC";
        case ND_PROGRAM:
            return "PROGRAM";
        default:
            return "?";
    }
}

static void indent(int depth) {
    for (int i = 0; i < depth; i++) printf("  ");
}

static void dump_node(Node *n, int depth);

static void dump_labeled(const char *label, Node *n, int depth) {
    if (!n) return;
    indent(depth);
    printf("%s:\n", label);
    dump_node(n, depth + 1);
}

static void dump_labeled_list(const char *label, Node *head, int depth) {
    if (!head) return;
    indent(depth);
    printf("%s:\n", label);
    for (Node *s = head; s; s = s->next) dump_node(s, depth + 1);
}

static void dump_node(Node *n, int depth) {
    if (!n) return;
    indent(depth);
    printf("%s", nd_name(n->kind));

    if (n->kind == ND_NUM) printf(" %d", n->val);
    if (n->kind == ND_CASE) printf(" %d", n->case_val);
    if (n->kind == ND_VAR || n->kind == ND_CALL || n->kind == ND_FUNC || n->kind == ND_MEMBER) printf(" '%s'", n->name);
    if (n->kind == ND_VAR_DECL && n->var) printf(" '%s'", n->var->name);
    if (n->kind == ND_STR) {
        printf(" ");
        emit_escaped_string(n->str_val);
    }
    if (n->ty) printf("  :: %s", type_name(n->ty));
    if (n->kind == ND_VAR && n->var && !n->var->is_global) printf("  [fp-%d]", n->var->offset);
    printf("\n");

    dump_node(n->lhs, depth + 1);
    dump_node(n->rhs, depth + 1);
    dump_labeled("cond", n->cond, depth + 1);
    dump_labeled("then", n->then, depth + 1);
    dump_labeled("else", n->els, depth + 1);
    dump_labeled("init", n->init, depth + 1);
    dump_labeled("inc", n->inc, depth + 1);
    dump_labeled("?", n->cond_true, depth + 1);
    dump_labeled(":", n->cond_false, depth + 1);
    dump_labeled_list("params", n->params, depth + 1);
    dump_labeled_list("args", n->args, depth + 1);
    dump_labeled_list("cases", n->cases, depth + 1);
    dump_labeled_list("body", n->body, depth + 1);
}

void dump_ast(Node *prog) { dump_node(prog, 0); }
