/**
 * Clings WASM Compiler
 * Uses browsercc's Emscripten modules (Clang/LLD) with clang (C) instead of clang++.
 * Caches module instances across compilations for fast repeated use.
 */

import { Clang, LLD, setUpSysroot } from './browsercc.js';

let clangInstance = null;
let lldInstance = null;
let sysrootData = null;
let sysrootSetUp = false;

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
      printErr: () => {},
    });
  }
  if (!lldInstance) {
    lldInstance = await LLD({
      thisProgram: "wasm-ld",
      printErr: () => {},
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

function setUpSysrootOnce(module, tar) {
  setUpSysroot(module, tar);
}

export async function compile({ source, fileName, flags, onProgress }) {
  if (onProgress) onProgress('Loading compiler...', 0);

  await ensureModules();
  const sysroot = await getSysroot();

  if (onProgress) onProgress('Analyzing...', 20);

  // Get compiler invocation (reuse existing clang instance)
  let invocationStderr = "";
  const origPrintErr = clangInstance.printErr;
  clangInstance.printErr = (data) => { invocationStderr += data + "\n"; };
  clangInstance.FS.writeFile(fileName, source);
  clangInstance.FS.mkdirTree("/lib/wasm32-wasi");
  clangInstance.FS.mkdirTree("/include");
  clangInstance.FS.writeFile("/lib/wasm32-wasi/crt1-command.o", new Uint8Array(0));
  clangInstance.FS.writeFile("/lib/wasm32-wasi/crt1-reactor.o", new Uint8Array(0));

  const ret = clangInstance.callMain([fileName, ...flags, "-###"]);
  clangInstance.printErr = origPrintErr;

  if (ret !== 0) {
    return { compileOutput: invocationStderr, module: null };
  }

  const lines = invocationStderr.split("\n");
  const getArgs = (key) => {
    const line = lines.find((l) => l.includes(key)) ?? "";
    const args = line.match(/"([^"]*)"/g).map((s) => s.slice(1, -1)).slice(1);
    const oIndex = args.findIndex((a) => a === "-o");
    return { args, outputFileName: args[oIndex + 1] };
  };
  const cc1 = getArgs("-cc1");
  const linker = getArgs("wasm-ld");

  if (onProgress) onProgress('Compiling...', 50);

  // Compile with sysroot
  let stderr = "";
  clangInstance.printErr = (data) => { stderr += data + "\n"; };
  setUpSysrootOnce(clangInstance, sysroot);

  let exitCode = clangInstance.callMain(cc1.args);
  if (exitCode !== 0) {
    clangInstance.printErr = origPrintErr;
    return { compileOutput: stderr, module: null };
  }
  const binary = clangInstance.FS.readFile(cc1.outputFileName, { encoding: "binary" });

  if (onProgress) onProgress('Linking...', 80);

  // Link
  lldInstance.FS.writeFile(cc1.outputFileName, binary);
  setUpSysrootOnce(lldInstance, sysroot);

  let linkStderr = "";
  lldInstance.printErr = (data) => { linkStderr += data + "\n"; };
  exitCode = lldInstance.callMain(linker.args);
  lldInstance.printErr = () => {};

  if (exitCode !== 0) {
    return { compileOutput: linkStderr, module: null };
  }
  const output = lldInstance.FS.readFile(linker.outputFileName, { encoding: "binary" });

  if (onProgress) onProgress('Done', 100);
  return {
    compileOutput: stderr + linkStderr,
    module: await WebAssembly.compile(output),
  };
}

export async function runWasm(wasmModule, stdin = '', timeout = 5) {
  const fsMem = await import('/static/wasm/fs_mem.js');
  const wasiShim = await import('/static/wasm/wasi.js');

  const WASI = wasiShim.default;
  const { File, OpenFile, ConsoleStdout } = fsMem;

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
