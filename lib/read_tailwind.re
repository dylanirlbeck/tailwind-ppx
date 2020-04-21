exception Tailwind_file_not_found;

let rec findFileTowardsRoot = (dir, file) => {
  let hereFile = Filename.concat(dir, file);
  print_endline("[read_tailwind][here_file] " ++ hereFile);

  if (Sys.file_exists(hereFile)) {
    let () = print_endline("[read_tailwind][found] " ++ hereFile);
    Some(hereFile);
  } else if (Filename.dirname(dir) == dir) {
    None;
  } else {
    findFileTowardsRoot(Filename.dirname(dir), file);
  };
};

/* lazily read tailwind and check if talwind.css file exists */
let getTailwind = () =>
  lazy(
    findFileTowardsRoot(Ppx_config.rootDirectory(), Ppx_config.tailwindFile())
  );
