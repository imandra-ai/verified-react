open Jest

let serverInfo = ref None

let module_name = "SimpleModel"

let syntax = Imandra_client.Syntax.Reason

let () =
  beforeAllPromise (fun () ->
      let si = (Imandra_client.ServerInfo.from_file ()) in
      serverInfo := Some (Imandra_client.ServerInfo.from_file ());
      Imandra_client.reset si
    )

let () =
  describe "simple counter model" (fun () ->

      beforeAllPromise (fun () ->
          let ip = !serverInfo |> Belt.Option.getExn in
          let model_path = Node.Path.join([|[%raw "__dirname"]; ".."; (Printf.sprintf "%s.ire" module_name) |]) in
          Imandra_client.Eval.by_src ip ~syntax ~src:(Printf.sprintf "#mod_use \"%s\"" model_path)
          |> Js.Promise.then_ (function
              | Belt.Result.Ok _ -> Js.Promise.resolve pass
              | Belt.Result.Error (e, _) -> begin
                  Js.Console.error(e);
                  Js.Promise.reject (Failure (Printf.sprintf "error from imandra: %s" e))
                end
            )
        );

      testPromise "verify increment" (fun () ->
          let ip = !serverInfo |> Belt.Option.getExn in
          let function_name = Imandra_client.function_name SimpleModel.goal_increment in
          Imandra_client.Verify.by_name ip ~name:(Printf.sprintf "%s.%s" module_name function_name)
          |> Js.Promise.then_ (function
              | Belt.Result.Ok (Imandra_client.Verify.Proved, _) ->
                Js.Promise.resolve pass
              | Belt.Result.Ok o -> begin
                  Js.Console.error(o);
                  Js.Promise.reject (Failure "unexpected result")
                end
              | Belt.Result.Error (e, _) -> begin
                  Js.Console.error(e);
                  Js.Promise.reject (Failure (Printf.sprintf "error from imandra: %s" e))
                end
            )
        );

      testPromise "verify decrement" (fun () ->
          let ip = !serverInfo |> Belt.Option.getExn in
          let function_name = Imandra_client.function_name SimpleModel.goal_decrement in
          Imandra_client.Verify.by_name ip ~name:(Printf.sprintf "%s.%s" module_name function_name)
          |> Js.Promise.then_ (function
              | Belt.Result.Ok (Imandra_client.Verify.Proved, _) ->
                Js.Promise.resolve pass
              | Belt.Result.Ok o -> begin
                  Js.Console.error(o);
                  Js.Promise.reject (Failure "unexpected result")
                end
              | Belt.Result.Error (e, _) -> begin
                  Js.Console.error(e);
                  Js.Promise.reject (Failure (Printf.sprintf "error from imandra: %s" e))
                end
            )
        )
    )
