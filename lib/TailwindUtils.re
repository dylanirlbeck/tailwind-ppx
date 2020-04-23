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
      | Rule.At_rule(_) => existingClassNames
      };
    };

    let names = List.fold_left(gatherClassSelector, [], fst(ast));
    acceptableNames := Some(names);
    names;
  };
};

// **********************************************************************

let splitClassNames = classNames => {
  String.split_on_char(' ', String.trim(classNames));
};

let rec validateClassNames = (splitClassNames, loc, tailwindFile) => {
  let validateClassName = className => {
    List.exists(
      acceptableClassName => acceptableClassName == className,
      getAcceptableClassNames(tailwindFile),
    );
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
