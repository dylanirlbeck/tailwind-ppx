open Css_types;
// ********************** CSS PARSER HELPERS *************************

let tailwindCss = {|
  .flex {
    display: flex;
  }

  .flex-row {
    flex-direction: row;
  }

  .mt-2 {
    margin-top: 0.5rem;
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

[@deriving show]
type declarationList = list(Declaration_list.kind);

[@deriving show]
type ast = list(Rule.t);

let getAcceptableClassNames = css => {
  //let stylesheet = parseStylesheet(css);
  print_string(css);
  ["flex", "flex-row", "mt-2"];
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
