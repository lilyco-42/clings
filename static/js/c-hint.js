/**
 * C language CodeMirror autocompletion
 * Inspired by code-server's Monaco IntelliSense approach
 */

// C keywords
const C_KEYWORDS = [
    'auto', 'break', 'case', 'char', 'const', 'continue', 'default', 'do',
    'double', 'else', 'enum', 'extern', 'float', 'for', 'goto', 'if',
    'inline', 'int', 'long', 'register', 'restrict', 'return', 'short',
    'signed', 'sizeof', 'static', 'struct', 'switch', 'typedef', 'union',
    'unsigned', 'void', 'volatile', 'while',
    '_Alignas', '_Alignof', '_Atomic', '_Bool', '_Complex', '_Generic',
    '_Imaginary', '_Noreturn', '_Static_assert', '_Thread_local',
    // Preprocessor
    '#include', '#define', '#ifdef', '#ifndef', '#endif', '#if', '#else',
    '#elif', '#undef', '#pragma', '#error', '#warning',
];

// Standard library functions with signatures
const C_STDLIB = [
    // stdio.h
    { text: 'printf', displayText: 'printf(format, ...)', output: 'printf($1)' },
    { text: 'fprintf', displayText: 'fprintf(stream, format, ...)', output: 'fprintf($1, $2)' },
    { text: 'sprintf', displayText: 'sprintf(str, format, ...)', output: 'sprintf($1, $2)' },
    { text: 'snprintf', displayText: 'snprintf(str, size, format, ...)', output: 'snprintf($1, $2, $3)' },
    { text: 'scanf', displayText: 'scanf(format, ...)', output: 'scanf($1)' },
    { text: 'fscanf', displayText: 'fscanf(stream, format, ...)', output: 'fscanf($1, $2)' },
    { text: 'sscanf', displayText: 'sscanf(str, format, ...)', output: 'sscanf($1, $2)' },
    { text: 'fopen', displayText: 'fopen(filename, mode)', output: 'fopen($1, $2)' },
    { text: 'fclose', displayText: 'fclose(stream)', output: 'fclose($1)' },
    { text: 'fread', displayText: 'fread(ptr, size, nmemb, stream)', output: 'fread($1, $2, $3, $4)' },
    { text: 'fwrite', displayText: 'fwrite(ptr, size, nmemb, stream)', output: 'fwrite($1, $2, $3, $4)' },
    { text: 'fgets', displayText: 'fgets(str, n, stream)', output: 'fgets($1, $2, $3)' },
    { text: 'fputs', displayText: 'fputs(str, stream)', output: 'fputs($1, $2)' },
    { text: 'puts', displayText: 'puts(str)', output: 'puts($1)' },
    { text: 'getchar', displayText: 'getchar()', output: 'getchar()' },
    { text: 'putchar', displayText: 'putchar(c)', output: 'putchar($1)' },
    { text: 'fseek', displayText: 'fseek(stream, offset, origin)', output: 'fseek($1, $2, $3)' },
    { text: 'ftell', displayText: 'ftell(stream)', output: 'ftell($1)' },
    { text: 'rewind', displayText: 'rewind(stream)', output: 'rewind($1)' },
    { text: 'fflush', displayText: 'fflush(stream)', output: 'fflush($1)' },
    { text: 'feof', displayText: 'feof(stream)', output: 'feof($1)' },
    { text: 'ferror', displayText: 'ferror(stream)', output: 'ferror($1)' },
    { text: 'perror', displayText: 'perror(str)', output: 'perror($1)' },

    // stdlib.h
    { text: 'malloc', displayText: 'malloc(size)', output: 'malloc($1)' },
    { text: 'calloc', displayText: 'calloc(nmemb, size)', output: 'calloc($1, $2)' },
    { text: 'realloc', displayText: 'realloc(ptr, size)', output: 'realloc($1, $2)' },
    { text: 'free', displayText: 'free(ptr)', output: 'free($1)' },
    { text: 'atoi', displayText: 'atoi(str)', output: 'atoi($1)' },
    { text: 'atof', displayText: 'atof(str)', output: 'atof($1)' },
    { text: 'atol', displayText: 'atol(str)', output: 'atol($1)' },
    { text: 'strtol', displayText: 'strtol(str, endptr, base)', output: 'strtol($1, $2, $3)' },
    { text: 'strtoul', displayText: 'strtoul(str, endptr, base)', output: 'strtoul($1, $2, $3)' },
    { text: 'strtod', displayText: 'strtod(str, endptr)', output: 'strtod($1, $2)' },
    { text: 'rand', displayText: 'rand()', output: 'rand()' },
    { text: 'srand', displayText: 'srand(seed)', output: 'srand($1)' },
    { text: 'abs', displayText: 'abs(x)', output: 'abs($1)' },
    { text: 'div', displayText: 'div(numer, denom)', output: 'div($1, $2)' },
    { text: 'exit', displayText: 'exit(status)', output: 'exit($1)' },
    { text: 'atexit', displayText: 'atexit(func)', output: 'atexit($1)' },
    { text: 'system', displayText: 'system(command)', output: 'system($1)' },
    { text: 'qsort', displayText: 'qsort(base, nmemb, size, compar)', output: 'qsort($1, $2, $3, $4)' },
    { text: 'bsearch', displayText: 'bsearch(key, base, nmemb, size, compar)', output: 'bsearch($1, $2, $3, $4, $5)' },

    // string.h
    { text: 'strlen', displayText: 'strlen(s)', output: 'strlen($1)' },
    { text: 'strcpy', displayText: 'strcpy(dest, src)', output: 'strcpy($1, $2)' },
    { text: 'strncpy', displayText: 'strncpy(dest, src, n)', output: 'strncpy($1, $2, $3)' },
    { text: 'strcat', displayText: 'strcat(dest, src)', output: 'strcat($1, $2)' },
    { text: 'strncat', displayText: 'strncat(dest, src, n)', output: 'strncat($1, $2, $3)' },
    { text: 'strcmp', displayText: 'strcmp(s1, s2)', output: 'strcmp($1, $2)' },
    { text: 'strncmp', displayText: 'strncmp(s1, s2, n)', output: 'strncmp($1, $2, $3)' },
    { text: 'strchr', displayText: 'strchr(s, c)', output: 'strchr($1, $2)' },
    { text: 'strrchr', displayText: 'strrchr(s, c)', output: 'strrchr($1, $2)' },
    { text: 'strstr', displayText: 'strstr(haystack, needle)', output: 'strstr($1, $2)' },
    { text: 'strtok', displayText: 'strtok(str, delim)', output: 'strtok($1, $2)' },
    { text: 'memcpy', displayText: 'memcpy(dest, src, n)', output: 'memcpy($1, $2, $3)' },
    { text: 'memmove', displayText: 'memmove(dest, src, n)', output: 'memmove($1, $2, $3)' },
    { text: 'memcmp', displayText: 'memcmp(s1, s2, n)', output: 'memcmp($1, $2, $3)' },
    { text: 'memset', displayText: 'memset(s, c, n)', output: 'memset($1, $2, $3)' },

    // math.h
    { text: 'sqrt', displayText: 'sqrt(x)', output: 'sqrt($1)' },
    { text: 'pow', displayText: 'pow(base, exp)', output: 'pow($1, $2)' },
    { text: 'sin', displayText: 'sin(x)', output: 'sin($1)' },
    { text: 'cos', displayText: 'cos(x)', output: 'cos($1)' },
    { text: 'tan', displayText: 'tan(x)', output: 'tan($1)' },
    { text: 'fabs', displayText: 'fabs(x)', output: 'fabs($1)' },
    { text: 'ceil', displayText: 'ceil(x)', output: 'ceil($1)' },
    { text: 'floor', displayText: 'floor(x)', output: 'floor($1)' },
    { text: 'log', displayText: 'log(x)', output: 'log($1)' },
    { text: 'log10', displayText: 'log10(x)', output: 'log10($1)' },
    { text: 'exp', displayText: 'exp(x)', output: 'exp($1)' },

    // ctype.h
    { text: 'isalpha', displayText: 'isalpha(c)', output: 'isalpha($1)' },
    { text: 'isdigit', displayText: 'isdigit(c)', output: 'isdigit($1)' },
    { text: 'isalnum', displayText: 'isalnum(c)', output: 'isalnum($1)' },
    { text: 'isspace', displayText: 'isspace(c)', output: 'isspace($1)' },
    { text: 'isupper', displayText: 'isupper(c)', output: 'isupper($1)' },
    { text: 'islower', displayText: 'islower(c)', output: 'islower($1)' },
    { text: 'toupper', displayText: 'toupper(c)', output: 'toupper($1)' },
    { text: 'tolower', displayText: 'tolower(c)', output: 'tolower($1)' },

    // stdio.h constants
    { text: 'NULL', displayText: 'NULL', output: 'NULL' },
    { text: 'EOF', displayText: 'EOF', output: 'EOF' },
    { text: 'stdin', displayText: 'stdin', output: 'stdin' },
    { text: 'stdout', displayText: 'stdout', output: 'stdout' },
    { text: 'stderr', displayText: 'stderr', output: 'stderr' },

    // stdlib.h constants
    { text: 'EXIT_SUCCESS', displayText: 'EXIT_SUCCESS', output: 'EXIT_SUCCESS' },
    { text: 'EXIT_FAILURE', displayText: 'EXIT_FAILURE', output: 'EXIT_FAILURE' },
    { text: 'RAND_MAX', displayText: 'RAND_MAX', output: 'RAND_MAX' },
    { text: 'BUFSIZ', displayText: 'BUFSIZ', output: 'BUFSIZ' },

    // Common patterns / snippets
    { text: 'main', displayText: 'int main(int argc, char *argv[])', output: 'int main(int argc, char *argv[]) {\n    $0\n    return 0;\n}' },
    { text: 'for_loop', displayText: 'for (int i = 0; i < n; i++)', output: 'for (int i = 0; i < $1; i++) {\n    $0\n}' },
    { text: 'while_loop', displayText: 'while (condition)', output: 'while ($1) {\n    $0\n}' },
    { text: 'if_else', displayText: 'if (condition) { ... } else { ... }', output: 'if ($1) {\n    $0\n} else {\n\n}' },
    { text: 'switch', displayText: 'switch (expression) { case ...: ... }', output: 'switch ($1) {\n    case $2:\n        $0\n        break;\n    default:\n        break;\n}' },
    { text: 'function', displayText: 'return_type name(params)', output: '$1 $2($3) {\n    $0\n    return $4;\n}' },
    { text: 'struct', displayText: 'struct Name { ... };', output: 'struct $1 {\n    $0\n};' },
    { text: 'enum', displayText: 'enum Name { ... };', output: 'enum $1 {\n    $0\n};' },
    { text: 'typedef', displayText: 'typedef existing_type new_type;', output: 'typedef $1 $2;' },
    { text: 'define', displayText: '#define NAME value', output: '#define $1 $2' },
    { text: 'include', displayText: '#include <header>', output: '#include <$1>' },
];

// Get cursor context for smarter completion
function getCursorContext(cm) {
    const cursor = cm.getCursor();
    const line = cm.getLine(cursor.line);
    const before = line.slice(0, cursor.ch);

    // Check if we're inside #include
    const includeMatch = before.match(/#include\s*[<"](\w*)$/);
    if (includeMatch) {
        return { type: 'include', partial: includeMatch[1] };
    }

    // Check if we're after a dot or arrow (struct member)
    const memberMatch = before.match(/(\w+)[\.\->](\w*)$/);
    if (memberMatch) {
        return { type: 'member', object: memberMatch[1], partial: memberMatch[2] };
    }

    // Check if we're inside a string
    const stringMatch = before.match(/"([^"]*)$/);
    if (stringMatch) {
        return { type: 'string', partial: stringMatch[1] };
    }

    // Default: identifier completion
    const idMatch = before.match(/(\w*)$/);
    return { type: 'identifier', partial: idMatch ? idMatch[1] : '' };
}

// Common headers for #include completion
const C_HEADERS = [
    'stdio.h', 'stdlib.h', 'string.h', 'math.h', 'ctype.h', 'assert.h',
    'limits.h', 'float.h', 'errno.h', 'stdint.h', 'stdbool.h', 'stddef.h',
    'time.h', 'stdarg.h', 'signal.h', 'setjmp.h', 'locale.h', 'complex.h',
    'fenv.h', 'inttypes.h', 'tgmath.h', 'wchar.h', 'wctype.h',
    'conio.h', 'process.h', 'direct.h', 'io.h',
];

// Custom C hint function
function cHint(cm) {
    const cursor = cm.getCursor();
    const token = cm.getTokenAt(cursor);
    const context = getCursorContext(cm);

    let completions = [];

    if (context.type === 'include') {
        // Complete header names
        completions = C_HEADERS
            .filter(h => h.startsWith(context.partial))
            .map(h => ({ text: h, displayText: h }));
    } else if (context.type === 'member') {
        // TODO: struct member completion (would need type info)
        return;
    } else if (context.type === 'string') {
        return;
    } else {
        // Regular identifier completion
        const partial = context.partial;

        // Keywords
        const keywords = C_KEYWORDS
            .filter(k => k.startsWith(partial) && k !== partial)
            .map(k => ({ text: k, displayText: k, className: 'cm-keyword' }));

        // Standard library functions
        const stdlib = C_STDLIB
            .filter(item => item.text.startsWith(partial) && item.text !== partial)
            .map(item => ({
                text: item.text,
                displayText: item.displayText,
                hint: function(cm, data, completion) {
                    cm.replaceRange(completion.output || completion.text, {
                        line: data.from.line,
                        ch: data.from.ch,
                    }, {
                        line: data.to.line,
                        ch: data.to.ch,
                    });
                    // Move cursor to first tab stop
                    const cursor = cm.getCursor();
                    const line = cm.getLine(cursor.line);
                    const tabPos = line.indexOf('$1');
                    if (tabPos !== -1) {
                        cm.setCursor({ line: cursor.line, ch: tabPos });
                        cm.replaceRange('', { line: cursor.line, ch: tabPos }, { line: cursor.line, ch: tabPos + 2 });
                    }
                },
            }));

        completions = [...keywords, ...stdlib];
    }

    if (completions.length === 0) return;

    return {
        list: completions,
        from: { line: cursor.line, ch: token.start },
        to: { line: cursor.line, ch: token.end },
    };
}

// Register hint helper
CodeMirror.registerHelper('hint', 'text/x-csrc', cHint);
CodeMirror.registerHelper('hint', 'text/x-c', cHint);
CodeMirror.registerHelper('hint', 'text/x-c++src', cHint);

// Auto-trigger hints on typing
CodeMirror.defaults.extraKeys = CodeMirror.defaults.extraKeys || {};
CodeMirror.defaults.extraKeys['Ctrl-Space'] = function(cm) {
    cm.showHint({ completeSingle: false });
};

// Auto-show hints on certain triggers
document.addEventListener('DOMContentLoaded', () => {
    // We'll set this up after the editor is created in app.js
    setTimeout(() => {
        if (window.editor) {
            window.editor.on('inputRead', function(cm, change) {
                // Don't auto-trigger on delete or paste
                if (change.removed && change.removed.length > 0) return;

                const cursor = cm.getCursor();
                const line = cm.getLine(cursor.line);
                const before = line.slice(0, cursor.ch);

                // Trigger on: after #include <, after . or ->, after any letter
                if (before.match(/#include\s*[<"]$/) ||
                    before.match(/[a-zA-Z_]$/) ||
                    before.match(/[\.\->]$/)) {
                    cm.showHint({ completeSingle: false, alignWithWord: true });
                }
            });
        }
    }, 500);
});
