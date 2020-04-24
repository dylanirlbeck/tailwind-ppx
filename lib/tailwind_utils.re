open Css_types;
module StringSet = Set.Make(String);

/********************* GENERAL HELPERS ************************/
/* Global ref to the acceptable class names. Using a ref so we don't parse on
 * each re-compile */
let acceptableNames = ref(None);

// Remove all the backslashes from identifiers
let unescapeIdent = ident => {
  Str.global_replace(Str.regexp({|\\|}), "", ident);
};

exception ParseError(string);

type closestClassName = {
  name: string,
  distance: int,
};

// Finds the class name closest to the given
let findClosest = (className, acceptableNames) => {
  let testCloser = (name, bestMatch) => {
    let distance = Levenshtein.distance(className, name);
    distance < bestMatch.distance ? {name, distance} : bestMatch;
  };
  List.fold_right(
    testCloser,
    acceptableNames,
    {name: "", distance: max_int},
  );
};

/********************** CSS PARSER HELPERS *************************/
let parseStylesheet = (~containerLnum=?, ~pos=?, css) =>
  try(
    Css_lexer.parse_string(
      ~container_lnum=?containerLnum,
      ~pos?,
      css,
      Css_parser.stylesheet,
    )
  ) {
  | _ =>
    raise(
      ParseError(
        "Your Tailwind CSS file could not be parsed. Please double-check to make sure it's valid CSS.",
      ),
    )
  };

let parseDeclarationList = (~containerLnum=?, ~pos=?, css) => {
  Css_lexer.parse_string(
    ~container_lnum=?containerLnum,
    ~pos?,
    css,
    Css_parser.declaration_list,
  );
};

let getAcceptableClassNames = css => {
  switch (acceptableNames^) {
  | Some(names) => names
  | None =>
    let ast = parseStylesheet(css);

    let gatherClassSelector = (existingClassNames, rule) => {
      switch (rule) {
      | Rule.Style_rule(styleRule) =>
        let prelude = fst(styleRule.prelude);
        switch (prelude) {
        | [
            (Component_value.Delim(_), _),
            (Component_value.Ident(ident), _),
          ] =>
          existingClassNames @ [unescapeIdent(ident)]
        | [
            (Component_value.Delim(_), _),
            (Component_value.Ident(ident), _),
            (Component_value.Delim(_), _),
            (Component_value.Ident("hover"), _),
          ] =>
          existingClassNames @ [unescapeIdent(ident)]
        | _ => existingClassNames // TODO add support for other preludes (if nec.)
        };
      | Rule.At_rule(_) => existingClassNames // TODO should we be skipping?
      };
    };

    let names =
      List.fold_left(gatherClassSelector, [], fst(ast)) |> StringSet.of_list;
    acceptableNames := Some(names);
    names;
  };
};

/********************  MAIN VALIDATION METHODS **************************/
let checkDuplicate = (classNames, loc) => {
  let classNamesSet = ref(StringSet.empty);

  let isDuplicate = className => {
    StringSet.mem(className, classNamesSet^)
      ? raise(
          Location.Error(
            Location.error(~loc, "Duplicate class name: " ++ className),
          ),
        )
      : classNamesSet := StringSet.add(className, classNamesSet^);
  };

  List.iter(isDuplicate, classNames);
};

let checkAcceptable = (classNames, loc, tailwindFile) => {
  let errorMessage = invalidClassName => {
    let closest =
      findClosest(
        invalidClassName,
        StringSet.elements(Option.unsafe_unwrap(acceptableNames^)),
      );

    Printf.sprintf(
      "Class name not found: %s. Did you mean %s?",
      invalidClassName,
      closest.name,
    );
  };
  // TODO add a suggested className as part of the error message here
  let isAcceptable = className => {
    StringSet.mem(className, getAcceptableClassNames(tailwindFile))
      ? ()
      : raise(
          Location.Error(Location.error(~loc, errorMessage(className))),
        );
  };

  List.iter(isAcceptable, classNames);
};

let getSplitClassNames = classNames => {
  List.filter(name => name != "", String.split_on_char(' ', classNames));
};

let validate = (classNames, loc, tailwindFile) => {
  let splitClassNames = getSplitClassNames(classNames);
  checkAcceptable(splitClassNames, loc, tailwindFile);
  checkDuplicate(splitClassNames, loc);
};

// ********************************************************************
