open Css_types;

// Remove all the backslashes from identifiers
let unescapeIdent = ident => {
  Str.global_replace(Str.regexp({|\\|}), "", ident);
};

// ********************** CSS PARSER HELPERS *************************

// TODO replace this with a File.read or something
// Mock CSS file
let tailwindCss = {|
      .flex {
        display: flex;
      }

      .flex-row {
        flex-direction: row;
      }

      .hover\:bg-mono-100:hover {
        background-color: #FFF;
      }
|};

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

exception UncaughtPrelude;

let getAcceptableClassNames = css => {
  //let stylesheet = parseStylesheet(css);
  let ast = parseStylesheet(css);

  let gatherClassSelector = (existingClassNames, rule) => {
    switch (rule) {
    | Rule.Style_rule(styleRule) =>
      let prelude = fst(styleRule.prelude);
      switch (prelude) {
      | [(Component_value.Delim(_), _), (Component_value.Ident(ident), _)] =>
        existingClassNames @ [unescapeIdent(ident)]
      | [
          (Component_value.Delim(_), _),
          (Component_value.Ident(ident), _),
          (Component_value.Delim(_), _),
          (Component_value.Ident("hover"), _),
        ] =>
        existingClassNames @ [unescapeIdent(ident)]
      | _ => raise(UncaughtPrelude)
      };
    | Rule.At_rule(_) => existingClassNames @ ["at_rule"]
    };
  };

  List.fold_left(gatherClassSelector, [], fst(ast));
};

// **********************************************************************

let splitClassNames = classNames => {
  String.split_on_char(' ', classNames);
};

let rec validateClassNames = (splitClassNames, loc) => {
  let validateClassName = className => {
    List.exists(
      acceptableClassName => acceptableClassName == className,
      getAcceptableClassNames(tailwindCss),
    );
  };
  switch (splitClassNames) {
  | [] => ()
  | [className, ...remainingClassNames] =>
    if (validateClassName(className)) {
      validateClassNames(remainingClassNames, loc);
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

let validate = (classNames, loc) => {
  let splitClasses = splitClassNames(classNames);
  validateClassNames(splitClasses, loc);
};
