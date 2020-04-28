open Migrate_parsetree;
open Ast_406;
open Ast_mapper;
open Parsetree;
open Utils;

let expr = (mapper, e) =>
  switch (e.pexp_desc) {
  | Pexp_extension((
      {txt: "tw", _},
      PStr([
        {
          pstr_desc:
            Pstr_eval(
              {
                pexp_loc: loc,
                // Force the PPX to only recognize strings inside quotations
                pexp_desc: Pexp_constant(Pconst_string(classNames, None)),
                _,
              },
              _,
            ),
          _,
        },
      ]),
    )) =>
    // Contents of tailwind.css file
    let tailwindFile =
      switch (Lazy.force(Read_tailwind.getTailwind())) {
      | Some(file) => file
      | None =>
        raise(
          Location.Error(
            Location.error(
              ~loc,
              "tailwind.css file not found in project hierarchy. You may need to manually set the path to the file with the -path argument.",
            ),
          ),
        )
      };

    validate(~classNames, ~loc, ~tailwindFile);

    Ast_helper.Exp.constant(Pconst_string(classNames, None));
  | _ => default_mapper.expr(mapper, e)
  };

// Default configuration
let () =
  Ppx_config.(
    setConfig({tailwindFile: "tailwind.css", rootDirectory: Sys.getcwd()})
  );

let args = [
  (
    "-path",
    Arg.String(
      tailwindFile =>
        Ppx_config.updateConfig(current => {...current, tailwindFile}),
    ),
    "<path/to/tailwind.css>",
  ),
];

let mapper = (_, _) => {...default_mapper, expr};

let () =
  Driver.register(~name="tailwind_ppx", ~args, Versions.ocaml_406, mapper);
