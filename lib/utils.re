open Css_types;
module StringSet = Set.Make(String);

/**
  * Splits a string on any whitespace into the individual class names
 */
let getSplitClassNames = classNames => {
  List.filter(
    name => String.trim(name) != "",
    Str.split(Str.regexp("[ \n\r\x0c\t]+"), classNames),
  )
  |> List.map(name => String.trim(name));
};

/** Remove all the backslashes from identifiers */
let unescapeIdent = ident => {
  Str.global_replace(Str.regexp({|\\|}), "", ident);
};

type closestClassName = {
  name: string,
  distance: int,
};

/** Finds the acceptable class name closest to the given invalid one */
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

exception ParseError(string);

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

/** Get all the classes from a given selector (prelude) */
let getClassesFromSelector = selector => {
  let rec getClasses = (classes, selector) => {
    switch (selector) {
    | [] => classes
    | [
        (Component_value.Delim("."), _),
        (Component_value.Ident(ident), _),
        ...rem,
      ] =>
      getClasses([ident, ...classes], rem)
    | [_, ...rem] => getClasses(classes, rem)
    };
  };

  getClasses([], selector);
};

/** Parses out the valid class names from the given CSS */
let getAcceptableClassNames = (cssRules, css) => {
  let rec gatherClassSelector = (existingClassNames, rule) => {
    switch (rule) {
    | Rule.Style_rule(styleRule) =>
      let prelude = fst(styleRule.prelude);
      switch (prelude) {
      | [
          (Component_value.Delim("."), _),
          (Component_value.Ident(_), _),
          ..._,
        ] =>
        List.fold_left(
          (classNames, classNameFromSelector) =>
            [unescapeIdent(classNameFromSelector), ...classNames],
          existingClassNames,
          getClassesFromSelector(prelude),
        )
      | _ => existingClassNames // Ignore other preludes
      };
    | Rule.At_rule({At_rule.name: ("media", _), At_rule.block, _}) =>
      switch (block) {
      | Stylesheet((rules, _)) =>
        List.fold_left(gatherClassSelector, existingClassNames, rules)
      | _ => []
      }
    // Ignore other prelude
    | _ => existingClassNames
    };
  };

  List.fold_left(gatherClassSelector, [], cssRules) |> StringSet.of_list;
};

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

let checkAcceptable = (classNames, loc, acceptableNames) => {
  let errorMessage = invalidClassName => {
    let closest =
      findClosest(invalidClassName, StringSet.elements(acceptableNames));

    Printf.sprintf(
      "Class name not found: %s. Did you mean %s?",
      invalidClassName,
      closest.name,
    );
  };

  let isAcceptable = className => {
    StringSet.mem(className, acceptableNames)
      ? ()
      : raise(
          Location.Error(Location.error(~loc, errorMessage(className))),
        );
  };

  List.iter(isAcceptable, classNames);
};

type cachedAcceptableClassNames = {
  tailwindCssHash: string,
  acceptableClassNames: StringSet.t,
};

let getCachedAcceptaleClassNames = (~filename, ~tailwindFileContent) =>
  switch (Pervasives.open_in(filename)) {
  | input =>
    print_endline("read from file cached");
    let fileContent: cachedAcceptableClassNames =
      Pervasives.input_value(input);
    Pervasives.close_in(input);
    if (Digest.string(tailwindFileContent) == fileContent.tailwindCssHash) {
      Some(fileContent.acceptableClassNames);
    } else {
      None;
    };
  | exception _ => None
  };

let acceptableClassNames = ref(None);
let validate = (~classNames, ~loc, ~tailwindFileContent) => {
  let filename = "activeClassNames.bin";

  let acceptableClassNames =
    switch (acceptableClassNames^) {
    | Some(value) => value
    | None =>
      switch (getCachedAcceptaleClassNames(~filename, ~tailwindFileContent)) {
      | Some(cachedAcceptableClassNames) =>
        acceptableClassNames := Some(cachedAcceptableClassNames);
        cachedAcceptableClassNames;
      | None =>
        let (cssRules, _) = parseStylesheet(tailwindFileContent);
        let calculatedAcceptableClassNames =
          getAcceptableClassNames(cssRules, tailwindFileContent);
        let output = Pervasives.open_out(filename);
        Pervasives.output_value(
          output,
          {
            tailwindCssHash: Digest.string(tailwindFileContent),
            acceptableClassNames: calculatedAcceptableClassNames,
          },
        );
        Pervasives.close_out(output);
        acceptableClassNames := Some(calculatedAcceptableClassNames);
        calculatedAcceptableClassNames;
      }
    };

  let splitClassNames = getSplitClassNames(classNames);
  checkAcceptable(splitClassNames, loc, acceptableClassNames);
  checkDuplicate(splitClassNames, loc);
};