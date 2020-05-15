type config = {
  tailwindFile: string,
  rootDirectory: string,
};

let configRef = ref(None);

let setConfig = config => configRef := Some(config);

let updateConfig = update => configRef := configRef^ |> Option.map(update);

let tailwindFile = () => (configRef^ |> Option.unsafe_unwrap).tailwindFile;

let rootDirectory = () => (configRef^ |> Option.unsafe_unwrap).rootDirectory;
