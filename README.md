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
  "ppx-flags": ["@dylanirlbeck/tailwind-ppx"]
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
  "ppx-flags": [ "esy x tailwind-ppx" ]
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

The following ReasonReact code will provide validation for your desired class names. See these examples:

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

## Configuration

### - path (TODO)

By default, `tailwind-ppx` looks for your `tailwind.css` file in the project root. If `tailwind.css` lives elsewhere, you'll need to specify the file path in your `bsconfig.json`.

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

### Releases

Releasing happens through GitHub actions. Once you update the version of the ppx in `esy.json` on `master`, GH actions will automatically publish a new release to `NPM` and `OPAM` once we create a new release on GitHub.

## Background/Sources
