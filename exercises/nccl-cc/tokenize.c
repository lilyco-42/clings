/*
 * tokenize.c - Lexical analyzer for nccl-cc
 *
 * Lesson 73: Recognizes int, main, void, return, numbers, (), {}, ;
 */
#include "nccl_cc.h"
#include <stdarg.h>

static int cur_line = 1;
static char *src_start;  /* beginning of source (for error reporting) */

static void error_at(char *loc, const char *fmt, ...)
{
    /* find line start */
    char *line_start = loc;
    while (line_start > src_start && line_start[-1] != '\n')
        line_start--;
    /* find line end */
    char *line_end = loc;
    while (*line_end && *line_end != '\n')
        line_end++;
    /* print line */
    int line_len = line_end - line_start;
    fprintf(stderr, "L%d: ", cur_line);
    fprintf(stderr, "%.*s\n", line_len, line_start);
    /* print caret */
    int pos = loc - line_start + 4; /* +4 for "L%d: " prefix approx */
    fprintf(stderr, "%*s^ ", pos, "");
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    exit(1);
}

static Token *new_token(int kind, char *loc, int len)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->loc = loc;
    tok->len = len;
    tok->line = cur_line;
    return tok;
}

static int starts_with(const char *p, const char *q)
{
    return strncmp(p, q, strlen(q)) == 0;
}

static int is_keyword(const char *s, int len)
{
    static const char *kw[] = {"int", "char", "void", "return", "if", "else",
                               "while", "for", "do", "break", "continue",
                               "struct", "sizeof", "switch", "case", "default",
                               "enum", NULL};
    for (int i = 0; kw[i]; i++)
        if ((int)strlen(kw[i]) == len && strncmp(kw[i], s, len) == 0)
            return 1;
    return 0;
}

/* Process escape character */
static int read_escaped_char(char **pp)
{
    char *p = *pp;
    int c;
    switch (*p) {
    case 'a': c = '\a'; break;
    case 'b': c = '\b'; break;
    case 't': c = '\t'; break;
    case 'n': c = '\n'; break;
    case 'v': c = '\v'; break;
    case 'f': c = '\f'; break;
    case 'r': c = '\r'; break;
    case '0': c = '\0'; break;
    case '\\': c = '\\'; break;
    case '\'': c = '\''; break;
    case '"': c = '"'; break;
    case 'x': {
        /* hex escape \xNN */
        p++;
        c = 0;
        while (isxdigit(*p)) {
            c = c * 16 + (isdigit(*p) ? *p - '0' : tolower(*p) - 'a' + 10);
            p++;
        }
        *pp = p;
        return c;
    }
    default:
        /* octal escape \NNN */
        if (*p >= '0' && *p <= '7') {
            c = 0;
            for (int i = 0; i < 3 && *p >= '0' && *p <= '7'; i++)
                c = c * 8 + (*p++ - '0');
            *pp = p;
            return c;
        }
        c = *p;
        break;
    }
    p++;
    *pp = p;
    return c;
}

Token *tokenize(char *input)
{
    char *p = input;
    src_start = input;
    cur_line = 1;
    Token head = {0};
    Token *cur = &head;

    while (*p) {
        if (*p == '\n') { cur_line++; p++; continue; }
        if (isspace(*p)) { p++; continue; }

        /* skip comments */
        if (starts_with(p, "//")) {
            while (*p && *p != '\n') p++;
            continue;
        }
        if (starts_with(p, "/*")) {
            char *q = strstr(p + 2, "*/");
            if (!q) { fprintf(stderr, "L%d: unclosed comment\n", cur_line); exit(1); }
            for (char *r = p; r < q + 2; r++) if (*r == '\n') cur_line++;
            p = q + 2;
            continue;
        }

        /* L86: string literal — decode escape sequences HERE so every
         * later stage (parser, codegen, runtime) sees the real bytes.
         * The token's loc points to an owned, decoded buffer. */
        if (*p == '"') {
            p++;
            char *buf = malloc(strlen(p) + 1); /* decoded ≤ raw length */
            int blen = 0;
            while (*p && *p != '"') {
                if (*p == '\\') {
                    p++;
                    buf[blen++] = (char)read_escaped_char(&p);
                } else {
                    buf[blen++] = *p++;
                }
            }
            if (*p == '"') p++;
            buf[blen] = '\0';
            cur->next = new_token(TK_STRING, buf, blen);
            cur = cur->next;
            continue;
        }

        /* L86: character literal (using read_escaped_char) */
        if (*p == '\'') {
            char *start = p++;
            int val;
            if (*p == '\\') {
                p++;
                val = read_escaped_char(&p);
            } else {
                val = *p++;
            }
            if (*p == '\'') p++;
            Token *tok = new_token(TK_NUM, start, p - start);
            tok->val = val;
            cur->next = tok;
            cur = tok;
            continue;
        }

        /* number */
        if (isdigit(*p)) {
            char *start = p;
            int val = strtol(p, &p, 0);
            Token *tok = new_token(TK_NUM, start, p - start);
            tok->val = val;
            cur->next = tok;
            cur = tok;
            continue;
        }

        /* identifier / keyword */
        if (isalpha(*p) || *p == '_') {
            char *start = p;
            while (isalnum(*p) || *p == '_') p++;
            int len = p - start;
            int kind = is_keyword(start, len) ? TK_KEYWORD : TK_IDENT;
            cur->next = new_token(kind, start, len);
            cur = cur->next;
            continue;
        }

        /* punctuation (multi-char: 3-char first, then 2-char) */
        static const char *mc3[] = {"<<=", ">>=", NULL};
        int found = 0;
        for (int i = 0; mc3[i]; i++) {
            if (starts_with(p, mc3[i])) {
                cur->next = new_token(TK_PUNCT, p, 3);
                cur = cur->next;
                p += 3;
                found = 1;
                break;
            }
        }
        if (found) continue;

        static const char *mc2[] = {"==","!=","<=",">=","&&","||","+=","-=","*=","/=","%=",
                                    "&=","|=","^=","<<",">>","++","--","->",NULL};
        for (int i = 0; mc2[i]; i++) {
            if (starts_with(p, mc2[i])) {
                cur->next = new_token(TK_PUNCT, p, 2);
                cur = cur->next;
                p += 2;
                found = 1;
                break;
            }
        }
        if (found) continue;

        /* punctuation (single-char) */
        if (strchr("+-*/%<>=!&|^~;,.?:(){}[]", *p)) {
            cur->next = new_token(TK_PUNCT, p, 1);
            cur = cur->next;
            p++;
            continue;
        }

        error_at(p, "unexpected character '%c'", *p);
    }

    cur->next = new_token(TK_EOF, p, 0);

    /* Post-pass: concatenate adjacent string literals
     * "hello" " world" → "hello world" */
    for (Token *t = head.next; t && t->next; t = t->next) {
        if (t->kind == TK_STRING && t->next->kind == TK_STRING) {
            int len1 = t->len;
            int len2 = t->next->len;
            char *merged = malloc(len1 + len2 + 1);
            memcpy(merged, t->loc, len1);
            memcpy(merged + len1, t->next->loc, len2);
            merged[len1 + len2] = '\0';
            t->loc = merged;
            t->len = len1 + len2;
            t->next = t->next->next; /* skip the second string token */
            /* stay on same token to check for more adjacent strings */
            continue;
        }
    }

    return head.next;
}

/* Emit a DECODED string as a double-quoted assembly literal,
 * re-escaping every byte GNU as cannot take raw. Counterpart of the
 * tokenizer's escape decoding: decode once at lex time, encode once
 * at emit time — no stage in between ever parses backslashes again. */
void emit_escaped_string(const char *s)
{
    putchar('"');
    for (const unsigned char *p = (const unsigned char *)s; *p; p++) {
        switch (*p) {
        case '\n': printf("\\n"); break;
        case '\t': printf("\\t"); break;
        case '\r': printf("\\r"); break;
        case '"':  printf("\\\""); break;
        case '\\': printf("\\\\"); break;
        default:
            if (*p < 32 || *p >= 127)
                printf("\\%03o", *p); /* octal escape for raw bytes */
            else
                putchar(*p);
        }
    }
    putchar('"');
}
