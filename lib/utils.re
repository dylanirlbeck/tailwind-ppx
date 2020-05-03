open Css_types;
module StringSet = Set.Make(String);

/********************* GENERAL HELPERS ************************/

/* Global ref to the acceptable class names. Using a ref so we don't parse on
 * each re-compile */
let acceptableNames = ref(None);

/* Splits a string on any whitespace into the individual class names */
let getSplitClassNames = classNames => {
  List.filter(
    name => String.trim(name) != "",
    Str.split(Str.regexp("[ \n\r\x0c\t]+"), classNames),
  )
  |> List.map(name => String.trim(name));
};

/* Remove all the backslashes from identifiers */
let unescapeIdent = ident => {
  Str.global_replace(Str.regexp({|\\|}), "", ident);
};

type closestClassName = {
  name: string,
  distance: int,
};

/* Finds the acceptable class name closest to the given invalid one */
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

/* Get all the classes from a given selector (prelude) */
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
}

/* Parses out the valid class names from the given CSS */
let getAcceptableClassNames = css => {
  // See if we've "cached" the acceptable names before
  switch (acceptableNames^) {
  | Some(names) => names
  | None =>
    let (cssRules, _) = parseStylesheet(css);

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
          existingClassNames
          @ List.map(unescapeIdent, getClassesFromSelector(prelude))
        | _ =>
          existingClassNames; // Ignore other precludes
        };
      | Rule.At_rule({At_rule.name: ("media", _), At_rule.block, _}) =>
        let atRuleClasses =
          switch (block) {
          | Stylesheet((rules, _)) =>
            List.fold_left(gatherClassSelector, [], rules)
          | _ => []
          };
        // Ignore prelude
        List.concat([existingClassNames, atRuleClasses]);
      | _ => existingClassNames
      };
    };

    let names =
      List.fold_left(gatherClassSelector, [], cssRules) |> StringSet.of_list;
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

  let isAcceptable = className => {
    StringSet.mem(className, getAcceptableClassNames(tailwindFile))
      ? ()
      : raise(
          Location.Error(Location.error(~loc, errorMessage(className))),
        );
  };

  List.iter(isAcceptable, classNames);
};

let validate = (~classNames, ~loc, ~tailwindFile) => {
  let splitClassNames = getSplitClassNames(classNames);
  checkAcceptable(splitClassNames, loc, tailwindFile);
  checkDuplicate(splitClassNames, loc);
};

// ********************************************************************
