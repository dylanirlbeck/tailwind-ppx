# tailwind-ppx

[![Actions Status](https://github.com/dylanirlbeck/tailwind-ppx/workflows/CI/badge.svg)](https://github.com/dylanirlbeck/tailwind-ppx/actions)
[![NPM Version](https://badge.fury.io/js/%40dylanirlbeck%2Ftailwind-ppx.svg)](https://badge.fury.io/js/%40dylanirlbeck%2Ftailwind-ppx)

Reason/OCaml PPX for writing compile-time validated Tailwind CSS classes.

```reason
<Component className=[%tw "flex flex-ro"]> // ERROR: Class name not found: flex-ro
  ...
</Component>
```

## Features

- 0 runtime cost
- Automatically watches for changes to your `tailwind.css` file (TODO)

## Installation

The most likely use case for `tailwind-ppx` is inside ReasonReact projects (using BuckleScript).

### With `npm` on Bucklescript projects

However, if using `esy` bothers you, we also provide a NPM package with prebuilt binaries.

```bash
yarn add --dev @dylanirlbeck/tailwind-ppx
# Or
npm install --dev @dylanirlbeck/tailwind-ppx
```

And add the PPX in your `bsconfig.json` file:

```json
{
  "ppx-flags": [ "@dylanirlbeck/tailwind-ppx"]
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

The following ReasonReact code will provide validation for your desired class names

```reason
<Component className=[%tw "flex flex-row]> // This is ok!
  ...
</Component>

<Component className=[%tw "flex flex-ro]> // ERROR: Class name not found: flex-ro
  ...
</Component>
```
