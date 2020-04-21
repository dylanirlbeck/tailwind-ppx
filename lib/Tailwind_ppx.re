open Migrate_parsetree;
open Ast_406;
open Ast_mapper;
open Parsetree;
open TailwindUtils;

let expr = (mapper, e) =>
  switch (e.pexp_desc) {
  /* If the expression is [%tw ""] */
  | Pexp_extension((
      {txt: "tw", _},
      PStr([
        {
          pstr_desc:
            Pstr_eval(
              {
                pexp_loc: loc,
                pexp_desc: Pexp_constant(Pconst_string(classNames, _delim)),
                _,
              },
              _,
            ),
          _,
        },
      ]),
    )) =>
    // TODO pull out `raise` calls into helper
    // Raise an error if there' no `tailwind.css` file available
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

    // Main validation function for all the class names in the string
    validate(classNames, loc, tailwindFile);

    // Use the same string provided to the PPX
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
