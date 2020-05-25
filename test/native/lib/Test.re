open Setup;
open Utils;
open Css.Types;
open Helpers;

describe("Main methods", ({test, _}) => {
  test("Parser works for base tailwind file", ({expect, _}) => {
    let baseTailwindCss = Read_tailwind.readFile("test/tailwind.css");
    parseStylesheet(baseTailwindCss) |> ignore;
    // TODO find a better way to make sure an error isn't thrown
    expect.bool(true).toBeTrue();
  });

  test("Parser handles @-webkit-keyframes", ({expect, _}) => {
    let css = {|
      @-webkit-keyframes hide {
  from {
    opacity: 0.8;
    visibility: visible;
  }

  to {
    opacity: 0;
    height: 0;
    visibility: hidden;
  }
}
    |};

    parseStylesheet(css) |> ignore;

    expect.bool(true).toBeTrue();
  });

  test("splitClassNames works with whitespace", ({expect, _}) => {
    let className = "flex flex-row ";

    expect.list(getSplitClassNames(className)).toEqual([
      "flex",
      "flex-row",
    ]);
  });

  test("splitClassNames works with whitespace", ({expect, _}) => {
    let className = "w-full h-full bg-white flex flex-col justify-center \n
      items-center";

    expect.list(getSplitClassNames(className)).toEqual([
      "w-full",
      "h-full",
      "bg-white",
      "flex",
      "flex-col",
      "justify-center",
      "items-center",
    ]);
  });

  test("splitClassNames works with whitespace", ({expect, _}) => {
    let className = "justify-center\nitems-center";

    expect.list(getSplitClassNames(className)).toEqual([
      "justify-center",
      "items-center",
    ]);
  });

  test("checkDuplicate throws correctly", ({expect, _}) => {
    let classNames = ["flex", "flex"];

    expect.fn(() => checkDuplicate(classNames, Location.none)).toThrow();
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

  describe("parseStylesheet works for different selectors", ({test, _}) => {
    test("Basic selectors", ({expect, _}) => {
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

    test("Hover selector", ({expect, _}) => {
      let tailwindCss = {|
      .hover\:bg-mono-100:hover {
        background-color: #FFF;
      }
    |};

      let ast = parseStylesheet(tailwindCss);

      let expected_ast = (
        [
          Rule.Style_rule({
            Style_rule.prelude: (
              [
                (Component_value.Delim("."), Location.none),
                (
                  Component_value.Ident("hover\\:bg-mono-100"),
                  Location.none,
                ),
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
});

describe(
  "getClassesFromSelector gets all classes in a given selector", ({test, _}) => {
  test("Basic selectors", ({expect, _}) => {
    let flex = [
      (Component_value.Delim("."), Location.none),
      (Component_value.Ident("flex"), Location.none),
    ];
    let expectedClassNames = ["flex"];
    expect.list(getClassesFromSelector(flex)).toEqual(expectedClassNames);
  });

  test("Hover selector", ({expect, _}) => {
    let hover = [
      (Component_value.Delim("."), Location.none),
      (Component_value.Ident("hover\\:bg-white"), Location.none),
      (Component_value.Delim(":"), Location.none),
      (Component_value.Ident("hover"), Location.none),
    ];
    let expectedClassNames = ["hover\\:bg-white"];
    expect.list(getClassesFromSelector(hover)).toEqual(expectedClassNames);
  });

  test("Multiple classnames with different pseudo classes", ({expect, _}) => {
    let hover = [
      (Component_value.Delim("."), Location.none),
      (Component_value.Ident("group"), Location.none),
      (Component_value.Delim(":"), Location.none),
      (Component_value.Ident("hover"), Location.none),
      (Component_value.Delim("."), Location.none),
      (Component_value.Ident("group-hover\\:bg-transparent"), Location.none),
    ];
    let expectedClassNames = ["group-hover\\:bg-transparent", "group"];
    expect.list(getClassesFromSelector(hover)).toEqual(expectedClassNames);
  });
});

describe(
  "getAcceptableClassNames works for different CSS selectors", ({test, _}) => {
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

  test("Responsive utilities", ({expect, _}) => {
    let tailwindCss = {|
    @media (min-width: 640px) {
      .sm\:text-5xl {
        font-size: 3rem;
      }
    }
    |};

    let expectedClassNames = ["sm:text-5xl"];
    expect.list(getAcceptableClassNames(tailwindCss) |> StringSet.elements).
      toEqual(
      expectedClassNames,
    );
  });

  test("Group hover", ({expect, _}) => {
    let tailwindCss = {|
    .group:hover .group-hover\:bg-transparent {
      background-color: transparent !imporant;
    }
    |};

    let expectedClassNames = ["group", "group-hover:bg-transparent"];
    expect.list(getAcceptableClassNames(tailwindCss) |> StringSet.elements).
      toEqual(
      expectedClassNames,
    );
  });
});
