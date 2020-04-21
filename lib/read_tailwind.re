// TODO should be using this custom exception
//exception Tailwind_file_not_found;

// TODO have a verbose logging flag
let rec findFileTowardsRoot = (dir, file) => {
  let hereFile = Filename.concat(dir, file);
  //print_endline("[read_tailwind][here_file] " ++ hereFile);

  if (Sys.file_exists(hereFile)) {
    //let () = print_endline("[read_tailwind][found] " ++ hereFile);
    Some(
      hereFile,
    );
  } else if (Filename.dirname(dir) == dir) {
    None;
  } else {
    findFileTowardsRoot(Filename.dirname(dir), file);
  };
};

// Reads the contents of a file
let readFile = path => {
  let ch = open_in(path);
  let s = really_input_string(ch, in_channel_length(ch));
  close_in(ch);
  s;
};

/* lazily read tailwind and check if talwind.css file exists */
let getTailwind = () =>
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
