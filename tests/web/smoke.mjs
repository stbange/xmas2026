// SPDX-License-Identifier: MIT
// Web smoke test: serves a web build directory and checks it boots in a real
// browser on a desktop and a mobile profile.
// Usage: node tests/web/smoke.mjs <web-dist-dir>
import http from "node:http";
import fs from "node:fs";
import path from "node:path";
import { createRequire } from "node:module";

const require = createRequire(import.meta.url);
const { chromium, devices } = require("playwright");

const dist = path.resolve(process.argv[2] || "build/web-playtest/web-dist");
const BOOT_TIMEOUT_MS = 30000;
const TYPES = {
  ".html": "text/html", ".js": "text/javascript", ".wasm": "application/wasm",
  ".data": "application/octet-stream", ".json": "application/json", ".png": "image/png",
};

function serve(dir) {
  const server = http.createServer((req, res) => {
    const url = new URL(req.url, "http://localhost");
    let file = path.join(dir, decodeURIComponent(url.pathname));
    if (!file.startsWith(dir)) { res.writeHead(403).end(); return; }
    if (fs.existsSync(file) && fs.statSync(file).isDirectory()) file = path.join(file, "index.html");
    fs.readFile(file, (err, data) => {
      if (err) { res.writeHead(404).end(); return; }
      res.writeHead(200, { "content-type": TYPES[path.extname(file)] || "application/octet-stream" });
      res.end(data);
    });
  });
  return new Promise((resolve) => server.listen(0, "127.0.0.1", () => resolve(server)));
}

async function boot(browser, label, contextOptions, query) {
  const context = await browser.newContext(contextOptions);
  const page = await context.newPage();
  const errors = [];
  const logs = [];
  page.on("pageerror", (e) => errors.push(e.message));
  page.on("console", (m) => {
    logs.push(m.text());
    if (m.type() === "error") errors.push(m.text());
  });
  await page.goto(`${baseUrl}/index.html${query}`);
  await page.waitForSelector("body[data-tb-ready='1']", { timeout: BOOT_TIMEOUT_MS });
  await page.waitForTimeout(500);
  const build = await page.textContent("#build");
  const canvas = await page.evaluate(() => {
    const c = document.getElementById("canvas");
    return { w: c.width, h: c.height };
  });
  const scrollW = await page.evaluate(() => document.documentElement.scrollWidth);
  const viewW = page.viewportSize().width;
  await context.close();
  const problems = [];
  if (errors.length) problems.push(`page errors: ${errors.join(" | ")}`);
  if (!/build [0-9a-z]+/.test(build || "")) problems.push(`build id not shown: '${build}'`);
  if (canvas.w <= 0 || canvas.h <= 0) problems.push(`canvas has no size: ${JSON.stringify(canvas)}`);
  if (scrollW > viewW) problems.push(`horizontal scroll: ${scrollW} > ${viewW}`);
  const playtestLogged = logs.some((l) => l.includes("[playtest]"));
  if (query.includes("playtest=1") !== playtestLogged) {
    problems.push(`URL parameters not passed to the game (playtest logged: ${playtestLogged})`);
  }
  console.log(`${problems.length ? "FAIL" : "ok  "} ${label}: ${build}, canvas ${canvas.w}x${canvas.h}`);
  return problems.map((p) => `${label}: ${p}`);
}

if (!fs.existsSync(path.join(dist, "index.html"))) {
  console.error(`no web build at ${dist}`);
  process.exit(2);
}
const server = await serve(dist);
const baseUrl = `http://127.0.0.1:${server.address().port}`;
const browser = await chromium.launch({ args: ["--use-gl=angle", "--use-angle=swiftshader", "--enable-unsafe-swiftshader"] });
let problems = [];
try {
  problems = problems.concat(await boot(browser, "desktop", { viewport: { width: 1280, height: 720 } }, ""));
  problems = problems.concat(await boot(browser, "desktop playtest", { viewport: { width: 1280, height: 720 } }, "?playtest=1"));
  problems = problems.concat(await boot(browser, "mobile", { ...devices["Pixel 7"] }, ""));
} finally {
  await browser.close();
  server.close();
}
if (problems.length) {
  for (const p of problems) console.error("ERROR:", p);
  process.exit(1);
}
console.log("web smoke test passed");
