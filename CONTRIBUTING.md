# Contributing

## Setup your development environment

You need Esy, you can install the latest version from [npm](https://npmjs.com):

```bash
yarn global add esy@latest
# Or
npm install -g esy@latest
```

Then run the `esy` command from this project root to install and build depenencies.

```bash
esy
```

This project uses [Dune](https://dune.build/) as a build system, if you add a dependency in your `package.json` file, don't forget to add it to your `dune` and `dune-project` files too.

### Running Tests

You can run the test compiled executable:

```bash
esy test
```

### Building documentation

Documentation for the libraries in the project can be generated with:

```bash
esy doc
open-cli $(esy doc-path)
```

This assumes you have a command like [open-cli](https://github.com/sindresorhus/open-cli) installed on your system.

> NOTE: On macOS, you can use the system command `open`, for instance `open $(esy doc-path)`

### Releasing

To create a release and publish it on Opam, first update the `CHANGES.md` file with the last changes and the version that you want to release.
The, you can run the script `script/release.sh`. The script will perform the following actions:

- Create a tag with the version found in `tailwind-ppx.opam`, and push it to your repository.
- Create the distribution archive.
- Publish the distribution archive to a Github Release.
- Update the repository's `gh-pages` branch with the latest documentation.
- Submit a PR on Opam's repository.

When the release is published on Github, the CI/CD will trigger the `Release` workflow which will perform the following actions

- Compile binaries for all supported platforms.
- Create an NPM release containing the pre-built binaries.
- Publish the NPM release to the registry.

### Repository Structure

The following snippet describes tailwind-ppx's repository structure.

```text
.
├── .github/
|   Contains Github specific files such as actions definitions and issue templates.
│
├── bin/
|   Source for tailwind-ppx's binary. This links to the library defined in `lib/`.
│
├── lib/
|   Source for tailwind-ppx's library. Contains tailwind-ppx's core functionnalities.
│
├── test/
|   Unit tests and integration tests for tailwind-ppx.
│
├── dune-project
|   Dune file used to mark the root of the project and define project-wide parameters.
|   For the documentation of the syntax, see https://dune.readthedocs.io/en/stable/dune-files.html#dune-project
│
├── LICENSE
│
├── package.json
|   Esy package definition.
|   To know more about creating Esy packages, see https://esy.sh/docs/en/configuration.html.
│
├── README.md
│
└── tailwind-ppx.opam
    Opam package definition.
    To know more about creating and publishing opam packages, see https://opam.ocaml.org/doc/Packaging.html.
```
