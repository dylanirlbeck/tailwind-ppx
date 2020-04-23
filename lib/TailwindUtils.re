open Css_types;

// Remove all the backslashes from identifiers
let unescapeIdent = ident => {
  Str.global_replace(Str.regexp({|\\|}), "", ident);
};

// ********************** CSS PARSER HELPERS *************************
let parseStylesheet = (~containerLnum=?, ~pos=?, css) => {
  Css_lexer.parse_string(
    ~container_lnum=?containerLnum,
    ~pos?,
    css,
    Css_parser.stylesheet,
  );
};

let parseDeclarationList = (~containerLnum=?, ~pos=?, css) => {
  Css_lexer.parse_string(
    ~container_lnum=?containerLnum,
    ~pos?,
    css,
    Css_parser.declaration_list,
  );
};

let acceptableNames = ref(None);
module StringSet = Set.Make(String);

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

// **********************************************************************

let splitClassNames = classNames => {
  List.filter(name => name != "", String.split_on_char(' ', classNames));
};

let rec validateClassNames = (splitClassNames, loc, tailwindFile) => {
  let validateClassName = className => {
    StringSet.mem(className, getAcceptableClassNames(tailwindFile));
  };
  switch (splitClassNames) {
  | [] => ()
  | [className, ...remainingClassNames] =>
    if (validateClassName(className)) {
      validateClassNames(remainingClassNames, loc, tailwindFile);
    } else {
      // TODO add a suggested className as part of the error message here
      raise(
        Location.Error(
          Location.error(~loc, "Class name not found: " ++ className),
        ),
      );
    }
  };
};

let validate = (classNames, loc, tailwindFile) => {
  let splitClasses = splitClassNames(classNames);
  validateClassNames(splitClasses, loc, tailwindFile);
};
