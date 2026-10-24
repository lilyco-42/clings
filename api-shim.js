/* Clings static demo shim —— 把 /api/* 请求在浏览器内应答，不依赖后端。
   数据来自 api-dump.json（离线导出的真实接口响应）。 */
(function () {
  var DUMP = null;
  var READY = fetch('api-dump.json').then(function (r) { return r.json(); }).then(function (d) { DUMP = d; return d; });
  var LS_KEY = 'clings-demo-progress';

  function ls() {
    try { return JSON.parse(localStorage.getItem(LS_KEY) || '{}'); } catch (e) { return {}; }
  }
  function saveLs(o) { try { localStorage.setItem(LS_KEY, JSON.stringify(o)); } catch (e) {} }

  function json(body, status) {
    return new Response(JSON.stringify(body), {
      status: status || 200,
      headers: { 'Content-Type': 'application/json; charset=utf-8' }
    });
  }

  var origFetch = window.fetch.bind(window);

  var compilerP = null;
  function getCompiler() {
    if (!compilerP) compilerP = import(new URL('static/wasm/clingswasm.js', document.baseURI).href);
    return compilerP;
  }

  // 用浏览器里的 WASM 编译器跑一遍用例，返回后端 /api/verify 的同构结果
  async function verifyInBrowser(name, init, d) {
    var detail = (d['/api/exercises/' + name]) || {};
    var body = {};
    try { body = JSON.parse((init && init.body) || '{}'); } catch (e) {}
    var source = body.source || '';
    var fileName = body.filename || detail.source || 'exercise.c';
    var res = { verified: false, exit_code: null, expected: detail.expected_return, stdout: '', stderr: '', compile_errors: '' };
    try {
      var m = await getCompiler();
      var out = await m.compile({
        source: source,
        fileName: fileName,
        flags: ['-std=c11', '-Wall', '-Wextra', '-pedantic']
      });
      if (!out.module) {
        res.compile_errors = out.compileOutput || '编译失败';
        return json(res);
      }
      var cases = (detail.cases && detail.cases.length) ? detail.cases : [{ stdin: '', stdout: null }];
      var lines = [];
      var allOk = true;
      var lastExit = 0;
      for (var i = 0; i < cases.length; i++) {
        var c = cases[i];
        var r = await m.runWasm(out.module, c.stdin || '', 5);
        lastExit = r.exit_code;
        if (detail.mode === 'stdout' && c.stdout != null) {
          var ok = (r.stdout || '').trim() === String(c.stdout).trim();
          allOk = allOk && ok;
          lines.push((ok ? '✓' : '✗') + ' 用例 ' + (i + 1) + (ok ? ' 通过' : ' 未通过'));
          lines.push('实际输出:');
          lines.push((r.stdout || '(空)').replace(/\s+$/, ''));
          if (!ok) {
            lines.push('期望输出:');
            lines.push(String(c.stdout).trim());
          }
        } else {
          lines.push('✓ 运行完成，退出码 ' + r.exit_code);
        }
        if (r.stderr) res.stderr += r.stderr;
      }
      res.exit_code = lastExit;
      res.stdout = lines.join('\n');
      if (detail.mode === 'stdout') {
        res.verified = allOk;
      } else {
        res.verified = (lastExit === (detail.expected_return == null ? 0 : detail.expected_return));
        if (!res.verified) res.stdout = '期望返回值: ' + detail.expected_return + '，实际: ' + lastExit;
      }
      return json(res);
    } catch (e) {
      res.compile_errors = 'WASM 编译异常: ' + String(e);
      return json(res);
    }
  }

  window.fetch = function (input, init) {
    var url = typeof input === 'string' ? input : (input && input.url) || '';
    var method = ((init && init.method) || (input && input.method) || 'GET').toUpperCase();
    var path;
    try { path = new URL(url, location.href).pathname.replace(/\/$/, ''); } catch (e) { path = url; }

    var m = path.match(/\/api\/.*$/);
    if (!m) return origFetch(input, init);
    var api = m[0];

    return READY.then(function (d) {
      // 验证：静态版没有服务端，用浏览器里的同一套 clang/lld 现场编译 + 跑用例
      var vm = api.match(/^\/api\/verify\/([^/]+)$/);
      if (vm && method === 'POST') {
        return verifyInBrowser(decodeURIComponent(vm[1]), init, d);
      }

      // 服务端编译/校验接口：静态版没有 GCC，提示用 WASM 模式
      if (/^\/api\/(compile|run|compile-and-run)/.test(api) && method === 'POST') {
        return json({ detail: '静态演示版没有服务端 GCC，请保持右上角 WASM 模式（默认已开启）再运行。' }, 400);
      }

      // 进度写入：落到 localStorage
      var pr = api.match(/^\/api\/progress\/([^/]+)(\/reset)?$/);
      if (pr) {
        var name = decodeURIComponent(pr[1]);
        var store = ls();
        if (pr[2]) { delete store[name]; saveLs(store); return json({ ok: true }); }
        if (method === 'POST') {
          var body = {};
          try { body = typeof (init && init.body) === 'string' ? JSON.parse(init.body) : (init && init.body) || {}; } catch (e) {}
          store[name] = body && body.state ? body.state : body;
          saveLs(store);
          return json({ ok: true, name: name });
        }
        return json(store[name] || null);
      }
      if (api === '/api/progress/reset' && method === 'POST') { saveLs({}); return json({ ok: true }); }
      if (api === '/api/progress/states') return json(ls());
      if (api === '/api/progress') return json(d['/api/progress']);

      // 其余 GET：查快照（先精确，再忽略查询串）
      var hit = d[api];
      if (hit === undefined && api.indexOf('?') >= 0) hit = d[api.split('?')[0]];
      if (hit === undefined && api === '/api/exercises') hit = d['/api/exercises'];
      if (hit !== undefined) {
        if (typeof hit === 'string') {
          return new Response(hit, { status: 200, headers: { 'Content-Type': 'text/plain; charset=utf-8' } });
        }
        return json(hit);
      }
      return json({ detail: '静态演示版未收录该接口: ' + api }, 404);
    });
  };

  // 静态模式下服务端赞助二维码走本地 assets
  var origOpen = window.open;
  window.open = function (u) {
    if (typeof u === 'string' && u.indexOf('/api/sponsor') === 0) return origOpen('./', '_blank');
    return origOpen.apply(window, arguments);
  };
})();
