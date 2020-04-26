# tailwind-ppx

[![Actions Status](https://github.com/dylanirlbeck/tailwind-ppx/workflows/CI/badge.svg)](https://github.com/dylanirlbeck/tailwind-ppx/actions)
[![NPM Version](https://badge.fury.io/js/%40dylanirlbeck%2Ftailwind-ppx.svg)](https://badge.fury.io/js/%40dylanirlbeck%2Ftailwind-ppx)

Reason/OCaml PPX for writing compile-time validated Tailwind CSS classes.

```reason
<Component className=[%tw "flex flex-ro"]> // ERROR: Class name not found: flex-ro. Did you mean flex-row?
  ...
</Component>
```

## Installation

The most likely use case for `tailwind-ppx` is inside ReasonReact projects (using BuckleScript). To get started, we recommend cloning our [demo project](https://github.com/dylanirlbeck/tailwind-ppx/tree/master/demo).

### With `yarn` or `npm` on Bucklescript projects

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
<Component className=[%tw "flex flex-row"] /> // This is ok!

// Example 2
<Component className=[%tw "flex flex-ro"] /> // ERROR: Class name not found: flex-ro. Did you mean flex-row?

// Example 3
<Component className=[%tw "flex flex-row flex"] /> // ERROR: Duplicate class name: flex
```

Note that this PPX requires your **generated** `tailwind.css` file to exist somewhere in the
project hierarchy. Though not required, it's recommended that you [configure the
path](#-path) to your `tailwind.css` file (relative to your project root).

### Moving or changing your `tailwind.css` file

If your `tailwind.css` file changes (or you move it) you'll need to rebuild your
project - for example, `bsb -clean-world` and `bsb -make-world` if in BuckleScript. 
At this time, `tailwind-ppx` does not automatically watch for changes, though this is on
the roadmap.

Alternatively, you can add the following rules to you bsconfig.json to re-trigger builds

```json
{
  "sources": [
    {
      "dir": "src",
      "subdirs": true,
      "generators": [
        {
          "name": "gen-tailwind",
          "edge": ["tailwind.css", ":", "styles.css"]
        }
      ]
    }
  ],
  "generators": [
    {
      "name": "gen-tailwind",
      "command": "tailwindcss build $in -o $out"
    }
  ]
}
```

You might have to specify the path to tailwind.css

```json
{
  "ppx-flags": [
    ["@dylanirlbeck/tailwind-ppx/tailwind-ppx", "-path ./src/tailwind.css"]
  ]
}
```

## Features
**Current**
* Invalid class names (and suggestions for valid ones!)
* Duplicate class names
* Always in-sync with your `tailwind.css` file (just make sure to re-build!)

**Upcoming**
* Redundant class names (like having both flex-row and flex-col)
* Automatic purging of unused class names

Have feature requests? Feel free to [open an issue](https://github.com/dylanirlbeck/tailwind-ppx/issues)!

## Configuration

### -path

By default, `tailwind-ppx` looks for your `tailwind.css` file in the root
directory. If `tailwind.css` lives elsewhere (or the name of your generated CSS file is different), you'll need to specify the file path in your `bsconfig.json`.

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

* [ocaml-css-parser](https://github.com/astrada/ocaml-css-parser)
* [styled-ppx](https://github.com/davesnx/styled-ppx)
* [graphql-ppx](https://github.com/reasonml-community/graphql_ppx)
