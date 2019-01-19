module I = Imandra_client

let from_base64 s =
  Node.Buffer.fromStringWithEncoding s `base64
  |> Node.Buffer.toString

let print_with_instance (m : I.Api.Response.with_instance) =
  Printf.sprintf "%s:\n\n%s"
    m.instance.type_
    (match m.instance.printed with
     | Some p -> p
     | None -> m.instance.model.src_base64 |> from_base64
    )

let print_with_unknown_reason (m : I.Api.Response.with_unknown_reason) =
  Printf.sprintf "reason: %s" m.unknown_reason

let toBeProved (res : (I.Api.Response.verify_result, I.Error.t) Belt.Result.t) =
  match res with
  | Belt.Result.Ok(I.Api.Response.V_proved) ->
    Jest.pass
  | Belt.Result.Ok(I.Api.Response.V_refuted x) ->
    Jest.fail (Format.asprintf "Expected: proved, Got: refuted\n\n%s" (print_with_instance x))
  | Belt.Result.Ok(I.Api.Response.V_unknown x) ->
    Jest.fail (Format.asprintf "Expected: proved, Got: unknown\n\n%s" (print_with_unknown_reason x))
  | Belt.Result.Error(e) ->
    Jest.fail (Format.asprintf "Expected: proved, Got: ERROR\n\n%a" I.Error.pp e)

let toBeUnknown (res : (I.Api.Response.verify_result, I.Error.t) Belt.Result.t) =
  match res with
  | Belt.Result.Ok(I.Api.Response.V_proved) ->
    Jest.fail (Format.asprintf "Expected: unknown, Got: proved")
  | Belt.Result.Ok(I.Api.Response.V_refuted x) ->
    Jest.fail (Format.asprintf "Expected: unknown, Got: refuted\n\n%s" (print_with_instance x))
  | Belt.Result.Ok(I.Api.Response.V_unknown _) ->
    Jest.pass
  | Belt.Result.Error(e) ->
    Jest.fail (Format.asprintf "Expected: unknown, Got: ERROR\n\n%a" I.Error.pp e)

let toBeRefuted (res : (I.Api.Response.verify_result, I.Error.t) Belt.Result.t) =
  match res with
  | Belt.Result.Ok(I.Api.Response.V_proved) ->
    Jest.fail (Format.asprintf "Expected: refuted, Got: proved")
  | Belt.Result.Ok(I.Api.Response.V_refuted _) ->
    Jest.pass
  | Belt.Result.Ok(I.Api.Response.V_unknown x) ->
    Jest.fail (Format.asprintf "Expected: refuted, Got: unknown\n\n%s" (print_with_unknown_reason x))
  | Belt.Result.Error(e) ->
    Jest.fail (Format.asprintf "Expected: refuted, Got: ERROR\n\n%a" I.Error.pp e)
