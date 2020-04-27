# tailwind-ppx starter

A starter project to play around with `tailwind-ppx` in a ReasonReact project.

## Run Project

Inside the project directory run the following:

```sh
yarn # Builds project dependencies
yarn re:watch # Compiles Reason and watches for file changes
```

and then in a separate shell run

```sh
yarn build:styles # Generates Tailwind CSS
yarn dev # Starts the server
```

> Note that `src/tailwind.css` is automatically generated from `src/index.css`.
> If you modify `src/index.css` you need to run `yarn build:styles` to
> re-generate your Tailwind CSS (and ensure `tailwind-ppx` is up-to-date).

<img src="./LandingPage.png" height="500" width="500">
