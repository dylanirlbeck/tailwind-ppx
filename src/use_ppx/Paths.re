module StringMap = Map.Make(String);

let projectRoot = ref("");
let bsbProjectRoot = ref("");

let bsconfig = "bsconfig.json";

let rec findProjectRoot = (~dir) =>
  if (Sys.file_exists(Filename.concat(dir, bsconfig))) {
    dir;
  } else {
    let parent = dir |> Filename.dirname;
    if (parent == dir) {
      prerr_endline(
        "Error: cannot find project root containing " ++ bsconfig ++ ".",
      );
      assert(false);
    } else {
      findProjectRoot(~dir=parent);
    };
  };

let setProjectRoot = () => {
  projectRoot := findProjectRoot(~dir=Sys.getcwd());
  bsbProjectRoot := projectRoot^;
};

let readDirsFromConfig = (~configSources) => {
  let dirs = ref([]);
  let root = projectRoot^;

  let rec processDir = (~subdirs, dir) => {
    let absDir = dir == "" ? root : Filename.concat(root, dir);
    if (Sys.file_exists(absDir) && Sys.is_directory(absDir)) {
      dirs := [dir, ...dirs^];
      if (subdirs) {
        absDir
        |> Sys.readdir
        |> Array.iter(d => processDir(~subdirs, Filename.concat(dir, d)));
      };
    };
  };

  let rec processSourceItem = (sourceItem: Ext_json_types.t) =>
    switch (sourceItem) {
    | Str(str) => str |> processDir(~subdirs=false)
    | Obj(map) =>
      switch (map |> StringMap.find_opt("dir")) {
      | Some(Str(str)) =>
        let subdirs =
          switch (map |> StringMap.find_opt("subdirs")) {
          | Some(True(_)) => true
          | Some(False(_)) => false
          | _ => false
          };
        str |> processDir(~subdirs);
      | _ => ()
      }
    | Arr(arr) => arr |> Array.iter(processSourceItem)
    | _ => ()
    };

  switch (configSources) {
  | Some(sourceItem) => processSourceItem(sourceItem)
  | None => ()
  };
  dirs^;
};

let readSourceDirs = (~configSources) => {
  let sourceDirs =
    ["lib", "bs", ".sourcedirs.json"]
    |> List.fold_left(Filename.concat, bsbProjectRoot^);
  let dirs = ref([]);

  let readDirs = json => {
    switch (json) {
    | Ext_json_types.Obj(map) =>
      switch (map |> StringMap.find_opt("dirs")) {
      | Some(Arr(arr)) =>
        arr
        |> Array.iter(x =>
             switch (x) {
             | Ext_json_types.Str(str) => dirs := [str, ...dirs^]
             | _ => ()
             }
           );
        ();
      | _ => ()
      }
    | _ => ()
    };
  };

  if (sourceDirs |> Sys.file_exists) {
    let jsonOpt = sourceDirs |> Ext_json_parse.parse_json_from_file;
    switch (jsonOpt) {
    | Some(json) =>
      if (bsbProjectRoot^ != projectRoot^) {
        readDirs(json);
        dirs := readDirsFromConfig(~configSources);
      } else {
        readDirs(json);
      }
    | None => ()
    };
  } else {
    print_endline("Warning: can't find source dirs " ++ sourceDirs);
    dirs := readDirsFromConfig(~configSources);
  };
  dirs^;
};
