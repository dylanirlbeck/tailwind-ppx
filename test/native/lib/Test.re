open Setup;
open TailwindUtils;
open Css_types;
open TestHelpers;

// Mock CSS file
let tailwindCss = {|
      .flex {
        display: flex;
      }

      .flex-row {
        flex-direction: row;
      }
|};

describe("Main test ", ({test, _}) => {
  test("", ({expect, _}) => {
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
  })
});
