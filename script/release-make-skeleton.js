#!/usr/bin/env node

const { execSync } = require("child_process");
const fs = require("fs");
const path = require("path");
const esy = require("../esy.json");

const filesToCopy = ["LICENSE", "README.md", "js/index.js"];

function exec(cmd) {
  console.log(`exec: ${cmd}`);
  return execSync(cmd).toString();
}

function mkdirpSync(p) {
  if (fs.existsSync(p)) {
    return;
  }
  mkdirpSync(path.dirname(p));
  fs.mkdirSync(p);
}

function removeSync(p) {
  exec(`rm -rf "${p}"`);
}

const src = path.resolve(path.join(__dirname, ".."));
const dst = path.resolve(path.join(__dirname, "..", "_release"));

removeSync(dst);
mkdirpSync(dst);

for (const file of filesToCopy) {
  const p = path.join(dst, file);
  mkdirpSync(path.dirname(p));
  fs.copyFileSync(path.join(src, file), p);
}

fs.copyFileSync(
  path.join(src, "script", "release-postinstall.js"),
  path.join(dst, "postinstall.js")
);

const filesToTouch = ["tailwind-ppx.exe", "use-tailwind-ppx.exe"];

for (const file of filesToTouch) {
  const p = path.join(dst, file);
  mkdirpSync(path.dirname(p));
  fs.writeFileSync(p, "");
}

const pkgJson = {
  name: "@dylanirlbeck/tailwind-ppx",
  version: esy.version,
  description: esy.description,
  homepage: esy.homepage,
  license: esy.license,
  repository: esy.repository,
  main: "js/index.js",
  scripts: {
    postinstall: "node postinstall.js"
  },
  bin: {
    "tailwind-ppx": "tailwind-ppx.exe",
    "use-tailwind-ppx": "use-tailwind-ppx.exe"
  },
  files: [
    "platform-windows-x64/",
    "platform-linux-x64/",
    "platform-darwin-x64/",
    "postinstall.js",
    "tailwind-ppx",
    "use-tailwind-ppx",
    "js"
  ]
};

fs.writeFileSync(
  path.join(dst, "package.json"),
  JSON.stringify(pkgJson, null, 2)
);
