// @flow

const { execSync } = require("child_process");
const fs = require("fs");
const path = require("path");
const package = require("../package.json");

const filesToCopy = ["LICENSE", "README.md", "CHANGES.md"];

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
  path.join(src, "scripts", "release-postinstall.js"),
  path.join(dst, "postinstall.js")
);

const filesToTouch = [
  "tailwind-ppx"
];

for (const file of filesToTouch) {
  const p = path.join(dst, file);
  mkdirpSync(path.dirname(p));
  fs.writeFileSync(p, "");
}

const pkgJson = {
  name: "@dylanirlbeck/tailwind-ppx",
  version: "%%VERSION%%",
  description: "A short, but powerful statement about your project",
  author: "Dylan Irlbeck",
  license: "MIT",
  homepage: "https://github.com/dylanirlbeck/tailwind-ppx",
  bugs: {
    url: "https://github.com/dylanirlbeck/tailwind-ppx/issues"
  },
  repository: {
    type: "git",
    url: "https://github.com/dylanirlbeck/tailwind-ppx.git"
  },
  scripts: {
    postinstall: "node postinstall.js"
  },
  bin: {
    tailwind-ppx: "tailwind-ppx"
  },
  files: [
    "platform-windows-x64/",
    "platform-linux-x64/",
    "platform-darwin-x64/",
    "postinstall.js",
    "tailwind-ppx"
  ]
};

fs.writeFileSync(path.join(dst, "package.json"), JSON.stringify(pkgJson, null, 2));
