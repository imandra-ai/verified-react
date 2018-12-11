open Jest

let runningImandraProcess = ref None

let () =
  beforeAllPromise ~timeout:10000 (fun () ->
      let open Imandra_client in
      Imandra_client.start (imandraOptions ~syntax:"reason" ~serverCmd:"imandra-http-server-dev" ())
      |> Js.Promise.then_ (fun ip ->
          runningImandraProcess := Some ip;
          Js.Promise.resolve ()
        )
    )

let () =
  describe "simple counter model" (fun () ->

      beforeAllPromise (fun () ->
          let ip = !runningImandraProcess |> Belt.Option.getExn in
          let model_path = Node.Path.join([|[%raw "__dirname"]; ".."; "SimpleModel.ire" |]) in
          Imandra_client.Eval.by_src ip ~src:(Printf.sprintf "#mod_use \"%s\"" model_path)
          |> Js.Promise.then_ (function
              | Belt.Result.Ok _ -> Js.Promise.resolve pass
              | Belt.Result.Error (e, _) -> Js.Promise.resolve (fail (Printf.sprintf "error from imandra: %s" e))
            )
        );

      testPromise "verify increment" (fun () ->
          let ip = !runningImandraProcess |> Belt.Option.getExn in
          Imandra_client.Verify.by_name ip ~name:"SimpleModel.goal_increment"
          |> Js.Promise.then_ (function
              | Belt.Result.Ok (Imandra_client.Verify.Proved, _) ->
                Js.Promise.resolve pass
              | _ ->
                Js.Promise.resolve (fail "unexpected result")
            )
        );

      testPromise "verify decrement" (fun () ->
          let ip = !runningImandraProcess |> Belt.Option.getExn in
          Imandra_client.Verify.by_name ip ~name:"SimpleModel.goal_decrement"
          |> Js.Promise.then_ (function
              | Belt.Result.Ok (Imandra_client.Verify.Proved, _) ->
                Js.Promise.resolve pass
              | _ ->
                Js.Promise.resolve (fail "unexpected result")
            )
        )
    )

let () =
  afterAllAsync (fun finish ->
      match !runningImandraProcess with
      | Some ip ->
        Imandra_client.stop ip
        |> Js.Promise.then_ (fun _ ->
            runningImandraProcess := None;
            finish ();
            Js.Promise.resolve ()
          )
        |> ignore
      | None ->
        finish();
    )
