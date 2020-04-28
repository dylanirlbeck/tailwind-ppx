open Css_types

let rec zip xs ys =
  match (xs, ys) with
  | [], _ -> []
  | _, [] -> []
  | x :: xs, y :: ys -> (x, y) :: zip xs ys

let eq_ast ast1 ast2 =
  let eq_list xs ys eq =
    List.length xs = List.length ys
    && List.fold_left (fun e (x, y) -> e && eq x y) true (zip xs ys)
  in
  let rec eq_component_value (cv1, _) (cv2, _) =
    let open Component_value in
    match (cv1, cv2) with
    | Paren_block b1, Paren_block b2 | Bracket_block b1, Bracket_block b2 ->
        eq_list b1 b2 eq_component_value
    | Percentage x1, Percentage x2
    | Ident x1, Ident x2
    | String x1, String x2
    | Uri x1, Uri x2
    | Operator x1, Operator x2
    | Delim x1, Delim x2
    | Hash x1, Hash x2
    | Number x1, Number x2
    | Unicode_range x1, Unicode_range x2 ->
        x1 = x2
    | Float_dimension x1, Float_dimension x2 -> x1 = x2
    | Dimension x1, Dimension x2 -> x1 = x2
    | Function ((n1, _), (b1, _)), Function ((n2, _), (b2, _)) ->
        n1 = n2 && eq_list b1 b2 eq_component_value
    | _ -> false
  and eq_at_rule r1 r2 =
    let n1, _ = r1.At_rule.name in
    let n2, _ = r2.At_rule.name in
    let pr1, _ = r1.At_rule.prelude in
    let pr2, _ = r2.At_rule.prelude in
    n1 = n2
    && eq_list pr1 pr2 eq_component_value
    &&
    match (r1.At_rule.block, r2.At_rule.block) with
    | Brace_block.Empty, Brace_block.Empty -> true
    | Brace_block.Declaration_list dl1, Brace_block.Declaration_list dl2 ->
        eq_declaration_list dl1 dl2
    | Brace_block.Stylesheet s1, Brace_block.Stylesheet s2 ->
        eq_stylesheet s1 s2
    | _ -> false
  and eq_declaration d1 d2 =
    let n1, _ = d1.Declaration.name in
    let n2, _ = d2.Declaration.name in
    let v1, _ = d1.Declaration.value in
    let v2, _ = d2.Declaration.value in
    let i1, _ = d1.Declaration.important in
    let i2, _ = d2.Declaration.important in
    n1 = n2 && eq_list v1 v2 eq_component_value && i1 = i2
  and eq_declaration_list (dl1, _) (dl2, _) =
    let eq_kind k1 k2 =
      match (k1, k2) with
      | Declaration_list.Declaration d1, Declaration_list.Declaration d2 ->
          eq_declaration d1 d2
      | Declaration_list.At_rule r1, Declaration_list.At_rule r2 ->
          eq_at_rule r1 r2
      | _ -> false
    in
    eq_list dl1 dl2 eq_kind
  and eq_style_rule r1 r2 =
    let pr1, _ = r1.Style_rule.prelude in
    let pr2, _ = r2.Style_rule.prelude in
    eq_list pr1 pr2 eq_component_value
    && eq_declaration_list r1.Style_rule.block r2.Style_rule.block
  and eq_rule r1 r2 =
    match (r1, r2) with
    | Rule.Style_rule r1, Rule.Style_rule r2 -> eq_style_rule r1 r2
    | Rule.At_rule r1, Rule.At_rule r2 -> eq_at_rule r1 r2
    | _ -> false
  and eq_stylesheet (st1, _) (st2, _) = eq_list st1 st2 eq_rule in
  eq_stylesheet ast1 ast2
