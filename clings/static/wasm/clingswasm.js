/**
 * Clings WASM Compiler
 * Uses browsercc's Emscripten modules (Clang/LLD) for C compilation in browser.
 */

import { Clang, LLD, setUpSysroot } from './browsercc.js';
import { File, OpenFile, ConsoleStdout } from './fs_mem.js';
import WASIShim from './wasi.js';

let clangInstance = null;
let lldInstance = null;
let sysrootData = null;

let _stderrTarget = null;
function stderrCapture(data) {
  if (_stderrTarget !== null) _stderrTarget += data + "\n";
}

async function getSysroot() {
  if (!sysrootData) {
    sysrootData = await (await fetch(new URL("sysroot.tar", import.meta.url).href)).arrayBuffer();
  }
  return sysrootData;
}

async function ensureModules() {
  if (!clangInstance) {
    clangInstance = await Clang({
      thisProgram: "clang",
      printErr: stderrCapture,
      noInitialRun: true,
      noExitRuntime: true,
    });
  }
  if (!lldInstance) {
    lldInstance = await LLD({
      thisProgram: "wasm-ld",
      printErr: stderrCapture,
      noInitialRun: true,
      noExitRuntime: true,
    });
  }
}

function resetFS(module) {
  try {
    const files = module.FS.readdir('/');
    for (const f of files) {
      if (f === '.' || f === '..' || f === 'dev' || f === 'proc' || f === 'tmp') continue;
      try { module.FS.unlink('/' + f); } catch {}
    }
  } catch {}
}

export async function compile({ source, fileName, flags, onProgress }) {
  if (onProgress) onProgress('Loading compiler...', 0);

  let lastError = null;
  for (let attempt = 0; attempt < 2; attempt++) {
    try {
      return await doCompile({ source, fileName, flags, onProgress });
    } catch (e) {
      lastError = e;
      console.error(`[WASM] attempt ${attempt + 1} failed:`, e.message);
      clangInstance = null;
      lldInstance = null;
      await ensureModules();
    }
  }
  return { compileOutput: `WASM error: ${lastError?.message || 'unknown'}`, module: null };
}

async function doCompile({ source, fileName, flags, onProgress }) {
  await ensureModules();
  const sysroot = await getSysroot();

  if (onProgress) onProgress('Analyzing...', 20);

  resetFS(clangInstance);
  resetFS(lldInstance);

  clangInstance.FS.writeFile(fileName, source);
  clangInstance.FS.mkdirTree("/lib/wasm32-wasi");
  clangInstance.FS.mkdirTree("/include");
  clangInstance.FS.writeFile("/lib/wasm32-wasi/crt1-command.o", new Uint8Array(0));
  clangInstance.FS.writeFile("/lib/wasm32-wasi/crt1-reactor.o", new Uint8Array(0));

  setUpSysroot(clangInstance, sysroot);

  _stderrTarget = "";
  const ret = clangInstance.callMain([fileName, ...flags, "-###"]);
  const invocationStderr = _stderrTarget;
  _stderrTarget = null;

  if (ret !== 0) {
    return { compileOutput: invocationStderr || "", module: null };
  }

  const lines = (invocationStderr || "").split("\n");
  const getArgs = (key) => {
    const line = lines.find((l) => l.includes(key)) ?? "";
    const matches = line.match(/"([^"]*)"/g);
    if (!matches) return { args: [], outputFileName: "" };
    const args = matches.map((s) => s.slice(1, -1)).slice(1);
    const oIndex = args.findIndex((a) => a === "-o");
    return { args, outputFileName: args[oIndex + 1] };
  };
  const cc1 = getArgs("-cc1");
  const linker = getArgs("wasm-ld");

  if (!cc1.outputFileName || !linker.outputFileName) {
    return { compileOutput: `Failed to parse compiler invocation:\n${invocationStderr}`, module: null };
  }

  if (onProgress) onProgress('Compiling...', 50);

  _stderrTarget = "";
  let exitCode = clangInstance.callMain(cc1.args);
  const stderr = _stderrTarget;
  _stderrTarget = null;

  if (exitCode !== 0) {
    return { compileOutput: stderr || "", module: null };
  }
  const binary = clangInstance.FS.readFile(cc1.outputFileName, { encoding: "binary" });

  if (onProgress) onProgress('Linking...', 80);

  setUpSysroot(lldInstance, sysroot);
  lldInstance.FS.writeFile(cc1.outputFileName, binary);

  _stderrTarget = "";
  exitCode = lldInstance.callMain(linker.args);
  const linkStderr = _stderrTarget;
  _stderrTarget = null;

  if (exitCode !== 0) {
    return { compileOutput: linkStderr || "", module: null };
  }
  const output = lldInstance.FS.readFile(linker.outputFileName, { encoding: "binary" });

  if (onProgress) onProgress('Done', 100);
  return {
    compileOutput: (stderr || "") + (linkStderr || ""),
    module: await WebAssembly.compile(output),
  };
}

export async function runWasm(wasmModule, stdin = '', timeout = 5) {
  const WASI = WASIShim.default || WASIShim;

  let stdout = '';
  let stderr = '';

  const stdinData = stdin
    ? new TextEncoder().encode(stdin + '\n')
    : new TextEncoder().encode('');

  const fds = [
    new OpenFile(new File(stdinData)),
    new ConsoleStdout((data) => { stdout += new TextDecoder().decode(data); }),
    new ConsoleStdout((data) => { stderr += new TextDecoder().decode(data); }),
  ];

  const wasi = new WASI([], [], fds);
  const instance = await WebAssembly.instantiate(wasmModule, {
    wasi_snapshot_preview1: wasi.wasiImport,
  });

  const runPromise = new Promise((resolve, reject) => {
    try {
      wasi.start(instance);
      resolve();
    } catch (e) {
      reject(e);
    }
  });

  const timeoutPromise = new Promise((_, reject) => {
    setTimeout(() => reject(new Error(`Timeout after ${timeout}s`)), timeout * 1000);
  });

  await Promise.race([runPromise, timeoutPromise]);

  return { stdout, stderr, exit_code: 0 };
}

export async function isCached() {
  return { clang: false, lld: false, sysroot: false, all: false };
}
