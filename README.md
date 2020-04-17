# tailwind-ppx

[![Actions Status](https://github.com/dylanirlbeck/tailwind-ppx/workflows/CI/badge.svg)](https://github.com/dylanirlbeck/tailwind-ppx/actions)
[![NPM Version](https://badge.fury.io/js/%40dylanirlbeck%2Ftailwind-ppx.svg)](https://badge.fury.io/js/%40dylanirlbeck%2Ftailwind-ppx)

A short, but powerful statement about your project

## Features

- Deploy prebuilt binaries to be consumed from Bucklescript projects

## Installation

### With `opam` on native projects

```bash
opam install tailwind-ppx
```

### With `esy` on native projects

```bash
esy add @opam/tailwind-ppx
```

### With `npm` on Bucklescript projects

The recommended way to use PPX libraries in Bucklescript projects is to use `esy`.

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
  "ppx-flags": [
    "ppx-flags": ["esy x tailwind-ppx"]
  ]
}
```

However, if using `esy` bothers you, we also provide a NPM package with prebuilt binaries.

```bash
yarn global add @dylanirlbeck/tailwind-ppx
# Or
npm -g install @dylanirlbeck/tailwind-ppx
```

And add the PPX in your `bsconfig.json` file:

```json
{
  "ppx-flags": [
    "ppx-flags": ["@dylanirlbeck/tailwind-ppx"]
  ]
}
```

## Usage

`tailwind_ppx` implements a ppx that transforms the `[%tailwind_ppx]` extension into an expression that adds 5 to the integer passed in parameter.

The code:

```ocaml
[%tailwind_ppx 5]
```

Will transform to something like:

```ocaml
5 + 5
```

## Contributing

Take a look at our [Contributing Guide](CONTRIBUTING.md).
