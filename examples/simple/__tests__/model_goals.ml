open Jest

let runningImandraProcess = ref None

let () =
  beforeAllAsync (fun finish ->
      let open Imandra_client in
      Imandra_client.start (imandraOptions ~syntax:"reason" ~serverCmd:"imandra-http-server-dev" ())
      |> Js.Promise.then_ (fun ip ->
          runningImandraProcess := Some ip;
          finish ();
          Js.Promise.resolve ()
        )
      |> ignore
    )

let () =
  describe "simple counter model" (fun () ->

      beforeAllPromise (fun () ->
          let ip = !runningImandraProcess |> Belt.Option.getExn in
          let model_path = Node.Path.join([|[%raw "__dirname"]; ".."; "model.re" |]) in
          Imandra_client.Eval.by_src ip ~src:(Printf.sprintf "#use \"%s\"" model_path)
          |> Js.Promise.then_ (function
              | Belt.Result.Ok _ -> Js.Promise.resolve pass
              | Belt.Result.Error (e, _) -> Js.Promise.resolve (fail (Printf.sprintf "error from imandra: %s" e))
            )
        );

      testPromise "verify increment" (fun () ->
          match !runningImandraProcess with
          | Some ip ->
            Imandra_client.Verify.by_name ip ~name:"goal_increment"
            |> Js.Promise.then_ (function
                | Belt.Result.Ok (Imandra_client.Verify.Proved, _) ->
                  Js.Promise.resolve pass
                | _ ->
                  Js.Promise.resolve (fail "unexpected result")
              )
          | None ->
            Js.Promise.reject (Failure "no imandra process available?")
        );

      testPromise "verify decrement" (fun () ->
          match !runningImandraProcess with
          | Some ip ->
            Imandra_client.Verify.by_name ip ~name:"goal_decrement"
            |> Js.Promise.then_ (function
                | Belt.Result.Ok (Imandra_client.Verify.Proved, _) ->
                  Js.Promise.resolve pass
                | _ ->
                  Js.Promise.resolve (fail "unexpected result")
              )
          | None ->
            Js.Promise.reject (Failure "no imandra process available?")
        )
    )

let () =
  afterAllAsync ~timeout:10000 (fun finish ->
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
        fail "no imandra process available during teardown?" |> ignore
    )
