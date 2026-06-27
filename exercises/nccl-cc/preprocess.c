/*
 * preprocess.c - Simple C preprocessor for nccl-cc
 *
 * Lesson 92: Handles #define (simple object-like macros) and #include "file"
 * This runs as a text-to-text transform BEFORE tokenization.
 *
 * Usage: called from main.c before tokenize()
 */
#include "nccl_cc.h"

#define MAX_MACROS 256
#define MAX_INPUT (1024 * 1024) /* 1MB max source */

struct Macro {
    char name[64];
    char value[256];
};

static struct Macro macros[MAX_MACROS];
static int nmacros = 0;

/* Pre-defined macros from command-line -D options */
#define MAX_PREDEFINES 64
static struct Macro predefines[MAX_PREDEFINES];
static int npredefines = 0;

static void add_macro(const char *name, const char *value) {
    strncpy(macros[nmacros].name, name, 63);
    strncpy(macros[nmacros].value, value, 255);
    nmacros++;
}

/* Called from main.c to register -D NAME=VALUE before preprocessing */
void preprocess_add_define(const char *name, const char *value) {
    if (npredefines >= MAX_PREDEFINES) return;
    strncpy(predefines[npredefines].name, name, 63);
    strncpy(predefines[npredefines].value, value ? value : "1", 255);
    npredefines++;
}

static const char *find_macro(const char *name) {
    for (int i = 0; i < nmacros; i++)
        if (strcmp(macros[i].name, name) == 0) return macros[i].value;
    return NULL;
}

/* Read a line from input, advance pointer. Returns NULL at end. */
static char *read_line(char **pp) {
    char *p = *pp;
    if (*p == '\0') return NULL;
    char *start = p;
    while (*p && *p != '\n') p++;
    if (*p == '\n') p++;
    *pp = p;
    /* return a copy */
    int len = p - start;
    char *line = malloc(len + 1);
    memcpy(line, start, len);
    line[len] = '\0';
    return line;
}

/* Expand macros in a line (simple word replacement).
 * Words inside string literals, character literals and // comments
 * must NOT be replaced — #define n 3 would otherwise corrupt "n=%d". */
static void expand_macros(char *line, char *out, int outsize) {
    char *p = line;
    char *o = out;
    char *end = out + outsize - 1;
    int in_str = 0; /* 0 outside, or the active quote char: '"' / '\'' */

    while (*p && o < end) {
        if (in_str) {
            if (*p == '\\' && p[1]) { /* copy escape sequences verbatim */
                *o++ = *p++;
                if (o < end) *o++ = *p++;
                continue;
            }
            if (*p == in_str) in_str = 0;
            *o++ = *p++;
            continue;
        }
        if (*p == '"' || *p == '\'') {
            in_str = *p;
            *o++ = *p++;
            continue;
        }
        if (*p == '/' && p[1] == '/') { /* rest of line is a comment */
            while (*p && o < end) *o++ = *p++;
            break;
        }
        if (isalpha(*p) || *p == '_') {
            char word[64];
            int wlen = 0;
            while ((isalnum(*p) || *p == '_') && wlen < 63) word[wlen++] = *p++;
            word[wlen] = '\0';

            const char *val = find_macro(word);
            if (val) {
                int vlen = strlen(val);
                if (o + vlen < end) {
                    memcpy(o, val, vlen);
                    o += vlen;
                }
            } else {
                if (o + wlen < end) {
                    memcpy(o, word, wlen);
                    o += wlen;
                }
            }
        } else {
            *o++ = *p++;
        }
    }
    *o = '\0';
}

/* Skip lines of a conditional block until the matching #else (when
 * stop_at_else is set, at nesting level 1) or the matching #endif.
 * Returns 1 when stopped at #else, 0 otherwise. */
static int skip_cond_block(char **pp, int stop_at_else) {
    int depth = 1;
    char *line;
    while ((line = read_line(pp)) != NULL) {
        char *sl = line;
        while (*sl == ' ' || *sl == '\t') sl++;
        if (strncmp(sl, "#ifdef", 6) == 0 || strncmp(sl, "#ifndef", 7) == 0) {
            depth++;
        } else if (strncmp(sl, "#else", 5) == 0 && depth == 1 && stop_at_else) {
            free(line);
            return 1;
        } else if (strncmp(sl, "#endif", 6) == 0) {
            depth--;
            if (depth == 0) {
                free(line);
                return 0;
            }
        }
        free(line);
    }
    return 0;
}

/* number of #ifdef/#ifndef branches we are currently EMITTING; when a
 * matching #else shows up, its alternative branch must be skipped */
static int cond_depth = 0;

char *preprocess(char *input) {
    char *output = malloc(MAX_INPUT);
    char *out = output;
    char *remaining = MAX_INPUT - 1 + output;
    char *p = input;
    char *line;
    int line_num = 0;

    nmacros = 0;
    cond_depth = 0;

    /* predefined macros */
    add_macro("__LINE__", "0");           /* will be updated per-line */
    add_macro("__FILE__", "\"<stdin>\""); /* a proper string literal */

    /* inject command-line -D macros */
    for (int i = 0; i < npredefines; i++) add_macro(predefines[i].name, predefines[i].value);

    while ((line = read_line(&p)) != NULL) {
        /* skip leading whitespace */
        char *l = line;
        while (*l == ' ' || *l == '\t') l++;

        /* #define NAME VALUE */
        if (strncmp(l, "#define", 7) == 0 && isspace(l[7])) {
            char *dp = l + 8;
            while (*dp == ' ' || *dp == '\t') dp++;
            char name[64];
            int nlen = 0;
            while ((isalnum(*dp) || *dp == '_') && nlen < 63) name[nlen++] = *dp++;
            name[nlen] = '\0';
            while (*dp == ' ' || *dp == '\t') dp++;
            /* value is rest of line (trim newline) */
            char value[256];
            int vlen = 0;
            while (*dp && *dp != '\n' && *dp != '\r' && vlen < 255) value[vlen++] = *dp++;
            value[vlen] = '\0';
            add_macro(name, value);
            free(line);
            continue;
        }

        /* #include "file" */
        if (strncmp(l, "#include", 8) == 0) {
            char *q = strchr(l, '"');
            if (q) {
                q++;
                char *qe = strchr(q, '"');
                if (qe) {
                    char filename[256];
                    snprintf(filename, 255, "%.*s", (int)(qe - q), q);
                    char *inc = read_file(filename);
                    if (inc) {
                        int ilen = strlen(inc);
                        if (out + ilen < remaining) {
                            memcpy(out, inc, ilen);
                            out += ilen;
                        }
                        free(inc);
                    }
                }
            }
            free(line);
            continue;
        }

        /* Update __LINE__ predefined macro */
        line_num++;
        {
            char lbuf[16];
            snprintf(lbuf, 15, "%d", line_num);
            /* update __LINE__ in macro table */
            for (int i = 0; i < nmacros; i++)
                if (strcmp(macros[i].name, "__LINE__") == 0) strncpy(macros[i].value, lbuf, 255);
        }

        /* #ifdef / #ifndef / #else / #endif — conditional compilation.
         * When the condition holds we emit the branch and remember it on
         * cond_depth, so the matching #else knows to SKIP its alternative
         * (previously both branches were emitted). When it fails we skip
         * ahead; stopping at #else means emitting the alternative. */
        if (strncmp(l, "#ifdef", 6) == 0 && isspace(l[6])) {
            char *dp = l + 7;
            while (*dp == ' ' || *dp == '\t') dp++;
            char cname[64];
            int cn = 0;
            while ((isalnum(*dp) || *dp == '_') && cn < 63) cname[cn++] = *dp++;
            cname[cn] = '\0';
            int defined = (find_macro(cname) != NULL);
            free(line);
            if (defined)
                cond_depth++;
            else if (skip_cond_block(&p, 1))
                cond_depth++; /* now emitting the #else branch */
            continue;
        }

        if (strncmp(l, "#ifndef", 7) == 0 && isspace(l[7])) {
            char *dp = l + 8;
            while (*dp == ' ' || *dp == '\t') dp++;
            char cname[64];
            int cn = 0;
            while ((isalnum(*dp) || *dp == '_') && cn < 63) cname[cn++] = *dp++;
            cname[cn] = '\0';
            int defined = (find_macro(cname) != NULL);
            free(line);
            if (!defined)
                cond_depth++;
            else if (skip_cond_block(&p, 1))
                cond_depth++; /* now emitting the #else branch */
            continue;
        }

        if (strncmp(l, "#else", 5) == 0) {
            /* the taken branch was just emitted — skip the alternative */
            free(line);
            if (cond_depth > 0) {
                skip_cond_block(&p, 0);
                cond_depth--;
            }
            continue;
        }

        if (strncmp(l, "#endif", 6) == 0) {
            if (cond_depth > 0) cond_depth--;
            free(line);
            continue;
        }

        /* #undef NAME */
        if (strncmp(l, "#undef", 6) == 0 && isspace(l[6])) {
            char *dp = l + 7;
            while (*dp == ' ' || *dp == '\t') dp++;
            char uname[64];
            int un = 0;
            while ((isalnum(*dp) || *dp == '_') && un < 63) uname[un++] = *dp++;
            uname[un] = '\0';
            /* remove from macro list */
            for (int i = 0; i < nmacros; i++) {
                if (strcmp(macros[i].name, uname) == 0) {
                    macros[i] = macros[--nmacros];
                    break;
                }
            }
            free(line);
            continue;
        }

        /* skip other unknown directives */
        if (*l == '#') {
            free(line);
            continue;
        }

        /* normal line: expand macros */
        char expanded[4096];
        expand_macros(line, expanded, sizeof(expanded));
        int elen = strlen(expanded);
        if (out + elen < remaining) {
            memcpy(out, expanded, elen);
            out += elen;
        }
        free(line);
    }

    *out = '\0';
    return output;
}
