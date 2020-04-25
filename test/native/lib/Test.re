open Setup;
open Tailwind_utils;
open Css_types;
open TestHelpers;

let {describe, describeSkip, describeOnly} =
  describeConfig
  |> withLifecycle(testLifecycle
       // Reset acceptable names before each run
       => testLifecycle |> beforeEach(() => acceptableNames := None))
  |> build;

describe("Main testing module", ({test, _}) => {
  test("Parser works for base tailwind file", ({expect, _}) => {
    let baseTailwindCss =
      Read_tailwind.readFile("test/native/lib/tailwind.css");
    parseStylesheet(baseTailwindCss) |> ignore;
    // TODO find a better way to make sure an error isn't thrown
    expect.bool(true).toBeTrue();
  });

  test("splitClassNames works with whitespace", ({expect, _}) => {
    let className = " flex-row  flex ";

    expect.list(getSplitClassNames(className)).toEqual([
      "flex-row",
      "flex",
    ]);
  });

  test("checkDuplicate throws correctly", ({expect, _}) => {
    let classNames = ["flex", "flex"];

    expect.fn(() => checkDuplicate(classNames, Location.none)).
      toThrowException(
      Location.Error(
        Location.error(~loc=Location.none, "Duplicate class name: flex"),
      ),
    );
  });

  test("levenshtein distance", ({expect, _}) => {
    let acceptableNames = ["flex", "flex-row", "hover:bg-mono-100"];
    let {name, _} = findClosest("bg-mono-100", acceptableNames);

    expect.string(name).toEqual("hover:bg-mono-100");
  });

  test("parseStylesheet throws correctly", ({expect, _}) => {
    let tailwindCss = {|
.flex {
  display: flex;
}

.flex-row {
  flex-direction: row;
  --asdfa
}
    |};

    expect.fn(() => parseStylesheet(tailwindCss)).toThrowException(
      ParseError(
        "Your Tailwind CSS file could not be parsed. Please double-check to make sure it's valid CSS.",
      ),
    );
  });

  test("correct AST for basic selectors", ({expect, _}) => {
    let tailwindCss = {|
      .flex {
        display: flex;
      }

      .flex-row {
        flex-direction: row;
      }
    |};

    let ast = parseStylesheet(tailwindCss);
    let expected_ast = (
      [
        Rule.Style_rule({
          Style_rule.prelude: (
            [
              (Component_value.Delim("."), Location.none),
              (Component_value.Ident("flex"), Location.none),
            ],
            Location.none,
          ),
          block: (
            [
              Declaration_list.Declaration({
                Declaration.name: ("display", Location.none),
                value: (
                  [(Component_value.Ident("flex"), Location.none)],
                  Location.none,
                ),
                important: (false, Location.none),
                loc: Location.none,
              }),
            ],
            Location.none,
          ),
          loc: Location.none,
        }),
        Rule.Style_rule({
          Style_rule.prelude: (
            [
              (Component_value.Delim("."), Location.none),
              (Component_value.Ident("flex-row"), Location.none),
            ],
            Location.none,
          ),
          block: (
            [
              Declaration_list.Declaration({
                Declaration.name: ("flex-direction", Location.none),
                value: (
                  [(Component_value.Ident("row"), Location.none)],
                  Location.none,
                ),
                important: (false, Location.none),
                loc: Location.none,
              }),
            ],
            Location.none,
          ),
          loc: Location.none,
        }),
      ],
      Location.none,
    );

    expect.bool(eq_ast(ast, expected_ast)).toBeTrue();
  });

  test("correct AST for hover", ({expect, _}) => {
    let tailwindCss = {|
      .hover\:bg-mono-100:hover {
        background-color: #FFF;
      }
    |};

    let ast = parseStylesheet(tailwindCss);

    //Css_fmt_printer.dump_stylesheet(Format.std_formatter, ast);

    let expected_ast = (
      [
        Rule.Style_rule({
          Style_rule.prelude: (
            [
              (Component_value.Delim("."), Location.none),
              (Component_value.Ident("hover\\:bg-mono-100"), Location.none),
              (Component_value.Delim(":"), Location.none),
              (Component_value.Ident("hover"), Location.none),
            ],
            Location.none,
          ),
          block: (
            [
              Declaration_list.Declaration({
                Declaration.name: ("background-color", Location.none),
                value: (
                  [(Component_value.Hash("FFF"), Location.none)],
                  Location.none,
                ),
                important: (false, Location.none),
                loc: Location.none,
              }),
            ],
            Location.none,
          ),
          loc: Location.none,
        }),
      ],
      Location.none,
    );

    expect.bool(eq_ast(ast, expected_ast)).toBeTrue();
  });
});

describe("Different types of AST nodes", ({test, _}) => {
  test("Basic selectors", ({expect, _}) => {
    let tailwindCss = {|
      .flex {
        display: flex;
      }

      .flex-row {
        flex-direction: row;
      }
    |};

    let expectedClassNames = ["flex", "flex-row"];
    expect.list(getAcceptableClassNames(tailwindCss) |> StringSet.elements).
      toEqual(
      expectedClassNames,
    );
  });

  test("Hover selector", ({expect, _}) => {
    let tailwindCss = {|
      .hover\:bg-mono-100:hover {
        background-color: #FFF;
      }
    |};

    let expectedClassNames = ["hover:bg-mono-100"];
    expect.list(getAcceptableClassNames(tailwindCss) |> StringSet.elements).
      toEqual(
      expectedClassNames,
    );
  });
});
