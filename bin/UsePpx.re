/** Reads the contents of a file */
let readFile = path => {
  let ch = open_in(path);
  let s = really_input_string(ch, in_channel_length(ch));
  close_in(ch);
  s;
};

/** Writes the contents of a file */
let writeFile = (path, newContents) => {
  let ch = open_out(path);
  output_string(ch, newContents);
  close_out(ch);
};

let numFilesTouched = ref(0);

let transformClassName = file => {
  let contents = readFile(file);
  let r = Str.regexp("className=\"\\(.*\\)\"");
  let newContents =
    Str.global_replace(r, "className=[%tw \"\\1\"]", contents);
  if (!Digest.equal(Digest.string(contents), Digest.string(newContents))) {
    numFilesTouched := numFilesTouched^ + 1;
  };

  writeFile(file, newContents);
};

let run = () => {
  print_endline("Running...");
  let (+++) = Filename.concat;
  Paths.setProjectRoot();

  let sourceDirs = Paths.readSourceDirs(~configSources=None);

  sourceDirs
  |> List.iter(sourceDir => {
       let files =
         switch (Sys.readdir(sourceDir) |> Array.to_list) {
         | files => files
         | exception (Sys_error(_)) => []
         };
       let reasonFiles =
         files |> List.filter(x => Filename.check_suffix(x, ".re"));

       reasonFiles
       |> List.iter(reasonFile => {
            let reasonFilePath = sourceDir +++ reasonFile;
            transformClassName(reasonFilePath);
          });
     });
  print_endline(
    "Number of files touched: " ++ string_of_int(numFilesTouched^),
  );
};

run();
