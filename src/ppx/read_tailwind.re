/** Recursively look for file starting at the project root */
let rec findFileTowardsRoot = (dir, file) => {
  let hereFile = Filename.concat(dir, file);

  if (Sys.file_exists(hereFile)) {
    Some(hereFile);
  } else if (Filename.dirname(dir) == dir) {
    None;
  } else {
    findFileTowardsRoot(Filename.dirname(dir), file);
  };
} /* Reads the contents of a file */;

/** Reads the contents of a file */
let readFile = path => {
  let ch = open_in(path);
  let s =
    try(really_input_string(ch, in_channel_length(ch))) {
    | End_of_file => ""
    };
  close_in(ch);
  s;
} /* lazily read tailwind and check if talwind.css file exists */;

/** Writes the contents of a file */
let writeFile = (path, newContents) => {
  let ch = open_out(path);
  output_string(ch, newContents);
  close_out(ch);
};

/** lazily read tailwind and check if talwind.css file exists */
let getTailwind =
  lazy(
    switch (
      findFileTowardsRoot(
        Ppx_config.rootDirectory(),
        Ppx_config.tailwindFile(),
      )
    ) {
    | Some(path) => Some(readFile(path))
    | None => None
    }
  );
