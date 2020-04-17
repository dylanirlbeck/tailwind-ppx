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
    print_string(classNames);
    validate(classNames, loc);
    /* Then replace by 42 */
    Ast_helper.Exp.constant(Pconst_string(classNames, None));
  | _ => default_mapper.expr(mapper, e)
  };

let mapper = (_, _) => {...default_mapper, expr};

let () = Driver.register(~name="tailwind_ppx", Versions.ocaml_406, mapper);
