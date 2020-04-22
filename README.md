# tailwind-ppx

[![Actions Status](https://github.com/dylanirlbeck/tailwind-ppx/workflows/CI/badge.svg)](https://github.com/dylanirlbeck/tailwind-ppx/actions)
[![NPM Version](https://badge.fury.io/js/%40dylanirlbeck%2Ftailwind-ppx.svg)](https://badge.fury.io/js/%40dylanirlbeck%2Ftailwind-ppx)

Reason/OCaml PPX for writing compile-time validated Tailwind CSS classes.

```reason
<Component className=[%tw "flex flex-ro"]> // ERROR: Class name not found: flex-ro
  ...
</Component>
```

## Installation

The most likely use case for `tailwind-ppx` is inside ReasonReact projects (using BuckleScript).

### With `npm` on Bucklescript projects

Install the PPX with `yarn` or `npm`

```bash
yarn add --dev @dylanirlbeck/tailwind-ppx
# Or
npm install --dev @dylanirlbeck/tailwind-ppx
```

And add the PPX in your `bsconfig.json` file:

```json
{
  "ppx-flags": ["@dylanirlbeck/tailwind-ppx/tailwind-ppx"]
}
```

You can also use `esy` to install the PPX inside your BuckleScript project.

Create an `esy.json` file with the content:

```json
{
  "name": "test_bs",
  "version": "0.0.0",
  "dependencies": {
    "@opam/tailwind-ppx": "*",
    "ocaml": "~4.6.1000"
  }
}
```

And add the PPX in your `bsconfig.json` file:

```json
{
  "ppx-flags": ["esy x tailwind-ppx"]
}
```

## Usage

`tailwind_ppx` implements a ppx that validates your Tailwind CSS classes at compile time.

For example, for the following (condensed) `tailwind.css` file:

```css
.flex {
  display: flex;
}

.flex-row {
  flex-direction: row;
}
```

`tailwind-ppx` will provide validation for your desired class names. See these
examples:

```reason
// Example 1
<Component className=[%tw "flex flex-row]> // This is ok!
  ...
</Component>

// Example 2
<Component className=[%tw "flex flex-ro]> // ERROR: Class name not found: flex-ro
  ...
</Component>
```

Note that this PPX requires a `tailwind.css` file to exist somewhere in the
project hierarchy. Though not required, it's recommended that you [configure the
path](#-path) to your `tailwind.css` file (relative to your project root).

### Moving or changing your `tailwind.css` file

If your `tailwind.css` file changes (or you move it) you'll need to rebuild your
project - for example, `yarn clean && yarn build` if in BuckleScript. At this
time, `tailwind-ppx` does not automatically watch for changes, though this is on
the roadmap.

## Features
**Current**
* Invalid class names
* Always in-sync with your `tailwind.css` file (just make sure to re-build!)

**Upcoming**
* Duplicate class names
* Redundant class names (like having both flex-row and flex-col)
* Support for expressions inside the PPX (like `[%tw Cn.make(...)]`) to allow variables and function calls

Have feature requests? Feel free to [open an issue](https://github.com/dylanirlbeck/tailwind-ppx/issues)!

## Configuration

### -path

By default, `tailwind-ppx` looks for your `tailwind.css` file in the root
directory. If `tailwind.css` lives elsewhere, you'll need to specify the file
path in your `bsconfig.json`.

```json
"ppx-flags": [
  ["@dylanirlbeck/tailwind-ppx", "-path ../path/to/tailwind.css",]
],
```

## Developing

### Relevant commands

- `esy build` -> Builds the project
- `esy watch` -> Watches for changes to Reason/OCaml files in the entire project, including in the `/test` directory
- `esy test_native` -> Runs the native tests (in `test/native`)
- `esy test_bs` -> Runs the BuckleScript tests (in `test/bucklescript`)

### Releasing

1. Bump the version of the ppx in `esy.json` on `master` (we use [semantic versioning](https://semver.org/))
2. Create and push a new tag
  ```
  $ git checkout master
  $ git tag vx.y.z
  $ git push origin vx.y.z
  ```
3. [Create detailed release notes](https://github.com/dylanirlbeck/tailwind-ppx/releases) for the new version, following the `Added/Changed/Fixed/Removed` format. Note that the new version of the PPX will automatically be pushed to NPM and a release will be created on GitHub. 

## Background/Sources
