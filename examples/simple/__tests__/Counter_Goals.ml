open Jest

module I = Imandra_client

let serverInfo = ref None

let module_name = "Counter"

let syntax = I.Api.Reason

let () =
  beforeAllPromise (fun () ->
      let si = (Imandra_client.Server_info.from_file () |> Belt.Result.getExn) in
      serverInfo := Some (si);
      Js.Promise.resolve ()
    )

let () =
  describe "simple counter model" (fun () ->
      beforeAllPromise (fun () ->
          let ip = !serverInfo |> Belt.Option.getExn in
          let model_path = Node.Path.join([|[%raw "__dirname"]; ".."; (Printf.sprintf "%s.ire" module_name) |]) in
          Imandra_client.Eval.by_src ip ~syntax ~src:(Printf.sprintf "#mod_use \"%s\"" model_path)
          |> Js.Promise.then_ (function
              | Belt.Result.Ok _ -> Js.Promise.resolve pass
              | Belt.Result.Error (e) -> begin
                  Js.Console.error(I.Error.pp_str e);
                  Js.Promise.reject (Failure (I.Error.pp_str e))
                end
            )
        );

      afterAllPromise
        (fun ()  -> Js.Promise.make (fun ~resolve ~reject ->
             (Js.Global.setTimeout (fun ()  ->
                  ((Imandra_client.reset
                      ((!serverInfo) |> Belt.Option.getExn))
                   |> (Js.Promise.then_ (function
                       | Belt.Result.Ok _ ->
                         (((resolve pass)[@bs ]); Js.Promise.resolve ())
                       | ((Belt.Result.Error (e))) ->
                         (Js.Console.error (I.Error.pp_str e);
                          ((reject
                              ((Failure ((I.Error.pp_str e)))))
                             [@bs ]);
                          Js.Promise.resolve ()))))
                  |> ignore) 500)
             |> ignore));

      testPromise "verify increment" (fun () ->
          let ip = !serverInfo |> Belt.Option.getExn in
          let function_name = Imandra_client.function_name Counter.goal_increment in
          Imandra_client.Verify.by_name ip ~name:(Printf.sprintf "%s.%s" module_name function_name)
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
          let function_name = Imandra_client.function_name Counter.goal_decrement in
          Imandra_client.Verify.by_name ip ~name:(Printf.sprintf "%s.%s" module_name function_name)
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
