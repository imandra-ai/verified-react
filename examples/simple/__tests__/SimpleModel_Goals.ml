open Jest

module I = Imandra_client

let serverInfo = ref None

let module_name = "SimpleModel"

let syntax = I.Api.Reason

let () =
  beforeAllPromise (fun () ->
      let si = (Imandra_client.ServerInfo.fromFile () |> Belt.Result.getExn) in
      serverInfo := Some (si);
      Imandra_client.reset si
    )

let () =
  describe "simple counter model" (fun () ->

      beforeAllPromise (fun () ->
          let ip = !serverInfo |> Belt.Option.getExn in
          let model_path = Node.Path.join([|[%raw "__dirname"]; ".."; (Printf.sprintf "%s.ire" module_name) |]) in
          Imandra_client.Eval.bySrc ip ~syntax ~src:(Printf.sprintf "#mod_use \"%s\"" model_path)
          |> Js.Promise.then_ (function
              | Belt.Result.Ok _ -> Js.Promise.resolve pass
              | Belt.Result.Error (e) -> begin
                  Js.Console.error(I.Error.pp_str e);
                  Js.Promise.reject (Failure (I.Error.pp_str e))
                end
            )
        );

      testPromise "verify increment" (fun () ->
          let ip = !serverInfo |> Belt.Option.getExn in
          let function_name = Imandra_client.function_name SimpleModel.goal_increment in
          Imandra_client.Verify.byName ip ~name:(Printf.sprintf "%s.%s" module_name function_name)
          |> Js.Promise.then_ (function
              | Belt.Result.Ok (I.Api.Response.V_proved) ->
                Js.Promise.resolve pass
              | Belt.Result.Ok o -> begin
                  Js.Console.error o;
                  Js.Promise.reject (Failure "unexpected result")
                end
              | Belt.Result.Error (e) -> begin
                  Js.Console.error(I.Error.pp_str e);
                  Js.Promise.reject (Failure (I.Error.pp_str e))
                end
            )
        );

      testPromise "verify decrement" (fun () ->
          let ip = !serverInfo |> Belt.Option.getExn in
          let function_name = Imandra_client.function_name SimpleModel.goal_decrement in
          Imandra_client.Verify.byName ip ~name:(Printf.sprintf "%s.%s" module_name function_name)
          |> Js.Promise.then_ (function
              | Belt.Result.Ok I.Api.Response.V_proved ->
                Js.Promise.resolve pass
              | Belt.Result.Ok o -> begin
                  Js.Console.error o;
                  Js.Promise.reject (Failure "unexpected result")
                end
              | Belt.Result.Error (e) -> begin
                  Js.Console.error(I.Error.pp_str e);
                  Js.Promise.reject (Failure (I.Error.pp_str e))
                end
            )
        )
    )
