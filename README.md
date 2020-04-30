# tailwind-ppx

[![Actions Status](https://github.com/dylanirlbeck/tailwind-ppx/workflows/CI/badge.svg)](https://github.com/dylanirlbeck/tailwind-ppx/actions)
[![NPM Version](https://badge.fury.io/js/%40dylanirlbeck%2Ftailwind-ppx.svg)](https://badge.fury.io/js/%40dylanirlbeck%2Ftailwind-ppx)

Reason/OCaml PPX for writing compile-time validated Tailwind CSS classes.

<p align="center"><img height="800" src="assets/demo.gif?raw=true" /></p>

## Installation

> For a new project, please check out the [demo](https://github.com/dylanirlbeck/tailwind-ppx-demo/blob/master/README.md) project. For integrating `tailwind-ppx` with existing projects, read on.

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

### Getting ready for production

As [outlined in the Tailwind docs](https://tailwindcss.com/docs/controlling-file-size/), when preparing for production you'll want to make sure that the only CSS from Tailwind that ends up in your bundle is CSS that you _actually use_ in your code.

First, take a second to read the [section on setting up Purgecss from the Tailwind docs](https://tailwindcss.com/docs/controlling-file-size/#setting-up-purgecss). In order to help with the process outlined in the docs, this package ships with a default extractor function that'll take care of ensuring that any CSS from Tailwind that you aren't using with this PPX can be purged from your production CSS bundle. You enable it by slightly modifiying the official example of how to set up your `postcss.config.js`:

```javascript
// postcss.config.js
const purgecss = require("@fullhuman/postcss-purgecss")({
  // Specify the paths to all ReasonML code where you're using this PPX.
  content: ["./src/**/*.re"],

  // Include the extractor from this package
  defaultExtractor: require("@dylanirlbeck/tailwind-ppx").extractor
});

module.exports = {
  plugins: [
    require("tailwindcss"),
    require("autoprefixer"),
    ...(process.env.NODE_ENV === "production" ? [purgecss] : [])
  ]
};
```

Doing this will ensure that you only ship CSS from Tailwind to production that you're actually using with this PPX.

### Conditional including of classes

This feature is out of scope for `tailwind-ppx`; instead, we recommend you  use [`re-classnames`](https://github.com/MinimaHQ/re-classnames). See the example below:

Here is an example:

```reason
module SomeComponent = {
  [@react.component]
  let make = (~someBool) => {
    let className =
      Cn.make([
        [%tw "text-blue-500"]->Cn.ifTrue(someBool),
        [%tw "text-gray-500"]->Cn.ifTrue(!someBool),
      ]);
    <div className />;
  };
};
```

## Features

**Current**

- Invalid class names (and suggestions for valid ones!)
- Duplicate class names
- Always in-sync with your `tailwind.css` file (just make sure to re-build!)
- Automatic purging of unused class names (with PurgeCSS and `tailwind-ppx`'s extractor function)

**Upcoming**

- Redundant class names (like having both flex-row and flex-col)
- Class name dependencies (like having `flex-row` without `flex`)

Have feature requests? Feel free to [open an issue](https://github.com/dylanirlbeck/tailwind-ppx/issues)!

## Configuration

### -path

By default, `tailwind-ppx` looks for your `tailwind.css` file in the root
directory. If `tailwind.css` lives elsewhere (or the name of your generated CSS file is different), you'll need to specify the file path in your `bsconfig.json`.

```json
"ppx-flags": [
  ["@dylanirlbeck/tailwind-ppx/tailwind-ppx", "-path ../path/to/tailwind.css",]
],
```

## Autocompletion (Neovim only)

If you're a Neovim user, you can download the [`coc-tailwindcss`](https://github.com/iamcco/coc-tailwindcss) extension to get class name autocompletion while using `tailwind-ppx` - just make sure to define a `tailwind.config.js` file. See the example below!

<img src="assets/autocompletion.png" height="600" width="800">

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

- [ocaml-css-parser](https://github.com/astrada/ocaml-css-parser)
- [styled-ppx](https://github.com/davesnx/styled-ppx)
- [graphql-ppx](https://github.com/reasonml-community/graphql_ppx)
