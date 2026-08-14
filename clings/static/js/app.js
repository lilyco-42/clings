/**
 * Clings - C语言练习 UI
 */

// API base URL
const API_BASE = '';

// Per-launch security token injected by the server into the page <head>.
const CLINGS_TOKEN = document.querySelector('meta[name="clings-token"]')?.content || '';

// State
let editor = null;
let currentExercise = null;
let exercises = [];
let wasmMode = true;
let wasmCompiler = null;
let wasmInitialized = false;

// WASM Compiler (lazy loaded)
let wasmLoadPromise = null;

function formatBytes(bytes) {
    if (bytes < 1024) return bytes + ' B';
    if (bytes < 1048576) return (bytes / 1024).toFixed(0) + ' KB';
    return (bytes / 1048576).toFixed(1) + ' MB';
}

async function initWasmCompiler() {
    if (wasmCompiler) return wasmCompiler;

    // Prevent parallel loads
    if (wasmLoadPromise) return wasmLoadPromise;

    wasmLoadPromise = (async () => {
        try {
            // Show progress toast
            const toast = showToast('Loading WASM compiler...', 'info', 0);

            wasmCompiler = await import('/static/wasm/clingswasm.js');

            // Check cache status
            const cached = await wasmCompiler.isCached();
            if (cached.all) {
                showToast('WASM compiler loaded (cached)', 'success', 2000);
            } else {
                // Will show progress during actual loads
                showToast('WASM compiler ready (first load - downloading ~95MB)', 'success', 3000);
            }

            wasmInitialized = true;
            if (toast) toast.remove();
            return wasmCompiler;
        } catch (e) {
            showToast(`WASM compiler load failed: ${e.message}`, 'error');
            wasmMode = false;
            updateWasmToggle();
            wasmLoadPromise = null;
            return null;
        }
    })();

    return wasmLoadPromise;
}

// Toast system (nvim-style status bar)
function showToast(message, type = 'info', duration = 4000) {
    const container = document.getElementById('toast-container');
    const toast = document.createElement('div');
    toast.className = `toast ${type}`;

    const icons = { error: '\u2716', warning: '\u26a0', success: '\u2714', info: '\u2139' };
    toast.innerHTML = `
        <span class="toast-icon">${icons[type] || icons.info}</span>
        <span class="toast-text">${escapeHtml(message)}</span>
        <span class="toast-dismiss" onclick="this.parentElement.remove()">\u2715</span>
    `;
    container.appendChild(toast);

    if (duration > 0) {
        setTimeout(() => {
            if (toast.parentElement) toast.remove();
        }, duration);
    }
    return toast;
}

function escapeHtml(s) {
    return s.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/\n/g, '<br>');
}

// Parse diagnostics and show toast summary
function showDiagnosticsToast(data) {
    const errs = data.error_count || 0;
    const warns = data.warning_count || 0;

    if (errs === 0 && warns === 0) return;

    // Build summary: "1 error, 2 warnings"
    const parts = [];
    if (errs > 0) parts.push(`${errs} error${errs > 1 ? 's' : ''}`);
    if (warns > 0) parts.push(`${warns} warning${warns > 1 ? 's' : ''}`);
    const summary = parts.join(', ');

    // First error message (short)
    let detail = '';
    if (data.errors && data.errors.length > 0) {
        const first = data.errors.find(e => e.severity === 'error' || e.severity === 'fatal error');
        if (first) {
            detail = ` | ${first.file}:${first.line} ${first.message}`;
            if (detail.length > 80) detail = detail.slice(0, 77) + '...';
        }
    }

    showToast(`${summary}${detail}`, errs > 0 ? 'error' : 'warning', 6000);
}

// DOM Elements
const exerciseList = document.getElementById('exercise-list');
const unitSelect = document.getElementById('unit-select');
const exerciseTitle = document.getElementById('exercise-title');
const exerciseDesc = document.getElementById('exercise-desc');
const hintBtn = document.getElementById('hint-btn');
const compileBtn = document.getElementById('compile-btn');
const runBtn = document.getElementById('run-btn');
const verifyBtn = document.getElementById('verify-btn');
const outputStdout = document.getElementById('output-stdout');
const outputStderr = document.getElementById('output-stderr');
const progressText = document.getElementById('progress-text');
const progressFill = document.getElementById('progress-fill');
const hintModal = document.getElementById('hint-modal');
const hintText = document.getElementById('hint-text');
const wasmToggle = document.getElementById('wasm-toggle');

// Toggle WASM mode
function toggleWasmMode() {
    wasmMode = !wasmMode;
    updateWasmToggle();
    if (wasmMode && !wasmInitialized) {
        initWasmCompiler();
    }
}

function updateWasmToggle() {
    if (wasmToggle) {
        wasmToggle.classList.toggle('active', wasmMode);
        wasmToggle.title = wasmMode ? 'WASM mode ON (browser compiler)' : 'WASM mode OFF (server compiler)';
    }
}

// Initialize CodeMirror
function initEditor() {
    editor = CodeMirror.fromTextArea(document.getElementById('code-editor'), {
        mode: 'text/x-csrc',
        theme: 'dracula',
        lineNumbers: true,
        matchBrackets: true,
        indentUnit: 4,
        tabSize: 4,
        indentWithTabs: false,
        lineWrapping: true,
        // Autocomplete
        extraKeys: {
            'Ctrl-Space': function(cm) { cm.showHint({ completeSingle: false }); },
            'Tab': function(cm) {
                if (cm.somethingSelected()) {
                    cm.indentSelection('add');
                } else {
                    cm.replaceSelection('    ', 'end');
                }
            },
            'Shift-Tab': function(cm) { cm.indentSelection('subtract'); },
        },
    });
    // Expose editor globally for c-hint.js
    window.editor = editor;
    editor.setValue('// 选择一个练习开始\nint main(void) {\n    return 0;\n}');
}

// API calls
async function fetchJSON(url, options = {}) {
    const method = (options.method || 'GET').toUpperCase();
    if (method !== 'GET') {
        options.headers = { ...(options.headers || {}), 'X-Clings-Token': CLINGS_TOKEN };
    }
    const response = await fetch(API_BASE + url, options);
    return response.json();
}

// Load exercises
async function loadExercises(unit = '') {
    const url = unit ? `/api/exercises?unit=${unit}` : '/api/exercises';
    const data = await fetchJSON(url);
    exercises = data.exercises || [];
    renderExerciseList();
}

// Load units
async function loadUnits() {
    const data = await fetchJSON('/api/units');
    const units = data.units || [];
    unitSelect.innerHTML = '<option value="">全部</option>';
    units.forEach(unit => {
        const option = document.createElement('option');
        option.value = unit.id;
        option.textContent = `${unit.id} - ${unit.title}`;
        unitSelect.appendChild(option);
    });
}

// Load progress
async function loadProgress() {
    const data = await fetchJSON('/api/progress');
    const { passed, total, percentage } = data;
    progressText.textContent = `${passed}/${total}`;
    progressFill.style.width = `${percentage}%`;
}

// Render exercise list
function renderExerciseList() {
    exerciseList.innerHTML = '';
    exercises.forEach(ex => {
        const li = document.createElement('li');
        li.dataset.name = ex.name;
        li.innerHTML = `
            <div class="exercise-name">${ex.name}</div>
            <div class="exercise-title">${ex.title}</div>
        `;
        li.addEventListener('click', () => selectExercise(ex));
        exerciseList.appendChild(li);
    });
}

// Select exercise
async function selectExercise(exercise) {
    currentExercise = exercise;

    // Update UI
    exerciseTitle.textContent = exercise.name;
    exerciseDesc.textContent = exercise.title;

    // Highlight in list
    document.querySelectorAll('.exercise-list li').forEach(li => {
        li.classList.remove('active');
        if (li.dataset.name === exercise.name) {
            li.classList.add('active');
        }
    });

    // Enable buttons
    compileBtn.disabled = false;
    runBtn.disabled = false;
    verifyBtn.disabled = false;
    hintBtn.style.display = 'inline-block';

    // Load source code
    try {
        const data = await fetchJSON(`/api/exercises/${exercise.name}/source`);
        if (data.source) {
            editor.setValue(data.source);
        }
    } catch (e) {
        console.error('Failed to load source:', e);
    }

    // Clear output
    clearOutput();
}

// Compile
async function compile() {
    if (!currentExercise) return;

    const source = editor.getValue();
    clearOutput();
    showOutput('编译中...', 'stdout');

    // WASM mode
    if (wasmMode) {
        try {
            const compiler = await initWasmCompiler();
            if (!compiler) {
                showOutput('WASM compiler not available, falling back to server', 'stderr');
                return compileServer(source);
            }

            let loadingToast = null;
            const result = await compiler.compile({
                source,
                fileName: currentExercise.source || 'exercise.c',
                flags: ['-std=c11', '-Wall', '-Wextra', '-pedantic'],
                onProgress: (label, pct, loaded, total) => {
                    if (loadingToast) loadingToast.remove();
                    const loadedStr = loaded ? formatBytes(loaded) : '';
                    const totalStr = total ? formatBytes(total) : '';
                    const msg = total ? `${label} ${loadedStr}/${totalStr} (${pct}%)` : `${label} ${pct}%`;
                    loadingToast = showToast(msg, 'info', 0);
                },
            });

            if (loadingToast) loadingToast.remove();

            if (result.module) {
                showOutput('编译成功 (WASM)', 'stdout');
                showToast('编译成功 (WASM)', 'success', 2000);
            } else {
                const detail = result.compileOutput || 'No output';
                console.error('[WASM compile] failed:', detail);
                showOutput(detail, 'stderr');

                const errLines = detail.split('\n').filter(l => l.includes(' error:'));
                const warnLines = detail.split('\n').filter(l => l.includes(' warning:'));
                const parts = [];
                if (errLines.length) parts.push(`${errLines.length} error`);
                if (warnLines.length) parts.push(`${warnLines.length} warning`);
                const summary = parts.length ? parts.join(', ') : '编译失败';
                const hint = errLines.length ? '\n' + errLines[0].trim() : '';
                showToast(summary + hint, 'error', 5000);
            }
        } catch (e) {
            console.error('[WASM compile] exception:', e);
            showOutput(`WASM error: ${e.message}\n${e.stack || ''}`, 'stderr');
            showToast(`WASM error: ${e.message}`, 'error');
        }
        return;
    }

    // Server mode
    return compileServer(source);
}

async function compileServer(source) {
    try {
        const data = await fetchJSON('/api/compile', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                source,
                filename: currentExercise.source || 'exercise.c',
            }),
        });

        if (data.success) {
            showOutput('编译成功', 'stdout');
            showToast('编译成功', 'success', 2000);
        } else {
            showOutput(data.stderr || '编译失败', 'stderr');
            showDiagnosticsToast(data);
        }
    } catch (e) {
        showOutput(`错误: ${e.message}`, 'stderr');
        showToast(`编译失败: ${e.message}`, 'error');
    }
}

// Run
async function run() {
    if (!currentExercise) return;

    const source = editor.getValue();
    clearOutput();
    showOutput('运行中...', 'stdout');

    // WASM mode
    if (wasmMode) {
        try {
            const compiler = await initWasmCompiler();
            if (!compiler) {
                showOutput('WASM compiler not available, falling back to server', 'stderr');
                return runServer(source);
            }

            let loadingToast = null;
            const compileResult = await compiler.compile({
                source,
                fileName: currentExercise.source || 'exercise.c',
                flags: ['-std=c11', '-Wall', '-Wextra', '-pedantic'],
                onProgress: (label, pct, loaded, total) => {
                    if (loadingToast) loadingToast.remove();
                    const loadedStr = loaded ? formatBytes(loaded) : '';
                    const totalStr = total ? formatBytes(total) : '';
                    const msg = total ? `${label} ${loadedStr}/${totalStr} (${pct}%)` : `${label} ${pct}%`;
                    loadingToast = showToast(msg, 'info', 0);
                },
            });

            if (loadingToast) loadingToast.remove();

            if (!compileResult.module) {
                const detail = compileResult.compileOutput || '编译失败';
                showOutput(detail, 'stderr');
                const errLines = detail.split('\n').filter(l => l.includes(' error:'));
                const warnLines = detail.split('\n').filter(l => l.includes(' warning:'));
                const parts = [];
                if (errLines.length) parts.push(`${errLines.length} error`);
                if (warnLines.length) parts.push(`${warnLines.length} warning`);
                const summary = parts.length ? parts.join(', ') : '编译失败';
                const hint = errLines.length ? '\n' + errLines[0].trim() : '';
                showToast(summary + hint, 'error', 5000);
                return;
            }

            showOutput('Running WASM module...', 'stdout');
            const runResult = await compiler.runWasm(compileResult.module, '', 5);

            if (runResult.stdout) showOutput(runResult.stdout, 'stdout');
            if (runResult.stderr) showOutput(runResult.stderr, 'stderr');
            if (runResult.exit_code !== 0) {
                showOutput(`退出码: ${runResult.exit_code}`, 'stderr');
                showToast(`运行错误 (exit ${runResult.exit_code})`, 'error');
            } else if (runResult.stdout) {
                showToast('运行成功', 'success', 2000);
            }
        } catch (e) {
            console.error('[WASM run] exception:', e);
            showOutput(`WASM error: ${e.message}\n${e.stack || ''}`, 'stderr');
            showToast(`WASM error: ${e.message}`, 'error');
        }
        return;
    }

    // Server mode
    return runServer(source);
}

async function runServer(source) {
    try {
        const data = await fetchJSON('/api/compile-and-run', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                source,
                filename: currentExercise.source || 'exercise.c',
            }),
        });

        if (data.stdout) showOutput(data.stdout, 'stdout');
        if (data.stderr) showOutput(data.stderr, 'stderr');
        if (data.exit_code !== undefined && data.exit_code !== 0) {
            showOutput(`退出码: ${data.exit_code}`, 'stderr');
        }
        if (data.error_count > 0 || data.warning_count > 0) {
            showDiagnosticsToast(data);
        }
    } catch (e) {
        showOutput(`错误: ${e.message}`, 'stderr');
        showToast(`运行失败: ${e.message}`, 'error');
    }
}

// Verify
async function verify() {
    if (!currentExercise) return;

    const source = editor.getValue();
    clearOutput();
    showOutput('验证中...', 'stdout');

    try {
        const data = await fetchJSON(`/api/verify/${currentExercise.name}`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                source,
                filename: currentExercise.source || 'exercise.c',
            }),
        });

        // Show compile errors
        if (data.compile_errors) {
            showOutput(data.compile_errors, 'stderr');
        }

        // Show stdout (includes test case results)
        if (data.stdout) {
            showOutput(data.stdout, 'stdout');
        }

        // Show stderr
        if (data.stderr) {
            showOutput(data.stderr, 'stderr');
        }

        if (data.verified) {
            showOutput('验证通过!', 'stdout');
            updateExerciseStatus(currentExercise.name, 'passed');
            showToast('验证通过', 'success', 3000);
        } else {
            if (data.expected !== undefined) {
                showOutput(`期望返回值: ${data.expected}, 实际: ${data.exit_code}`, 'stderr');
            } else if (data.case_results && data.case_results.length > 0) {
                const failed = data.case_results.filter(c => !c.passed).length;
                showOutput(`验证失败 - ${failed}/${data.case_results.length} 测试用例未通过`, 'stderr');
            } else {
                showOutput('验证失败', 'stderr');
            }
            updateExerciseStatus(currentExercise.name, 'failed');

            // Show diagnostics toast if compile errors
            if (data.error_count > 0 || data.warning_count > 0) {
                showDiagnosticsToast(data);
            } else {
                showToast('验证失败', 'error', 3000);
            }
        }

        // Refresh progress
        loadProgress();
    } catch (e) {
        showOutput(`错误: ${e.message}`, 'stderr');
        showToast(`验证失败: ${e.message}`, 'error');
    }
}

// Show hint
async function showHint() {
    if (!currentExercise) return;

    try {
        const data = await fetchJSON(`/api/exercises/${currentExercise.name}/hint`);
        hintText.textContent = data.hint || '暂无提示';
        hintModal.style.display = 'flex';
    } catch (e) {
        hintText.textContent = '获取提示失败';
        hintModal.style.display = 'flex';
    }
}

// Update exercise status in list
function updateExerciseStatus(name, status) {
    const li = document.querySelector(`.exercise-list li[data-name="${name}"]`);
    if (li) {
        li.classList.remove('passed', 'failed');
        li.classList.add(status);
    }
}

// Output helpers
function showOutput(text, type = 'stdout') {
    const el = type === 'stderr' ? outputStderr : outputStdout;
    el.textContent += text + '\n';
}

function clearOutput() {
    outputStdout.textContent = '';
    outputStderr.textContent = '';
}

// Tab switching
document.querySelectorAll('.tab').forEach(tab => {
    tab.addEventListener('click', () => {
        document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
        document.querySelectorAll('.output-content').forEach(c => c.classList.remove('active'));

        tab.classList.add('active');
        document.getElementById(`output-${tab.dataset.tab}`).classList.add('active');
    });
});

// Modal close
document.querySelector('.close').addEventListener('click', () => {
    hintModal.style.display = 'none';
});

hintModal.addEventListener('click', (e) => {
    if (e.target === hintModal) {
        hintModal.style.display = 'none';
    }
});

// Event listeners
unitSelect.addEventListener('change', (e) => loadExercises(e.target.value));
hintBtn.addEventListener('click', showHint);
compileBtn.addEventListener('click', compile);
runBtn.addEventListener('click', run);
verifyBtn.addEventListener('click', verify);
if (wasmToggle) wasmToggle.addEventListener('click', toggleWasmMode);

// Keyboard shortcuts
document.addEventListener('keydown', (e) => {
    if (e.ctrlKey || e.metaKey) {
        if (e.key === 'Enter') {
            e.preventDefault();
            verify();
        } else if (e.key === 's') {
            e.preventDefault();
            compile();
        }
    }
});

// Initialize
document.addEventListener('DOMContentLoaded', () => {
    initEditor();
    loadUnits();
    loadExercises();
    loadProgress();
    initSidebarToggle();
    // Pre-load WASM compiler in background (default mode)
    if (wasmMode) initWasmCompiler();
});

// Sidebar toggle (responsive)
function initSidebarToggle() {
    const toggle = document.getElementById('sidebar-toggle');
    const sidebar = document.querySelector('.sidebar');
    const overlay = document.getElementById('sidebar-overlay');

    if (!toggle || !sidebar) return;

    toggle.addEventListener('click', () => {
        sidebar.classList.toggle('open');
    });

    if (overlay) {
        overlay.addEventListener('click', () => {
            sidebar.classList.remove('open');
        });
    }

    // Close sidebar when an exercise is selected (on mobile)
    exerciseList.addEventListener('click', (e) => {
        if (window.innerWidth <= 900 && e.target.closest('li')) {
            sidebar.classList.remove('open');
        }
    });
}
