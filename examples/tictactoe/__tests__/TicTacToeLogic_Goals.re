open Jest;

let runningImandraProcess = ref(None);

let () =
  beforeAllPromise(() =>
    Imandra_client.(
      Imandra_client.start(
        imandraOptions(
          ~syntax="reason",
          ~serverCmd="imandra-http-server-dev",
          (),
        ),
      )
      |> Js.Promise.then_(ip => {
           runningImandraProcess := Some(ip);
           Js.Promise.resolve();
         })
    )
  );

let () =
  describe("tic tac toe model", () => {
    beforeAllPromise(~timeout:10000, () => {
      let ip = runningImandraProcess^ |> Belt.Option.getExn;
      let model_path =
        Node.Path.join([|[%raw "__dirname"], "..", "TicTacToeLogic.ire"|]);
      Imandra_client.Eval.by_src(
        ip,
        ~src=Printf.sprintf("#mod_use \"%s\"", model_path),
      )
      |> Js.Promise.then_(
           fun
           | Belt.Result.Ok(_) => Js.Promise.resolve(pass)
           | Belt.Result.Error((e, _)) =>
             Js.Promise.resolve(
               fail(Printf.sprintf("error from imandra: %s", e)),
             ),
         );
    });
    testPromise("verify increment", () => {
      let ip = runningImandraProcess^ |> Belt.Option.getExn;
      Imandra_client.Verify.by_name(
        ip,
        ~name="TicTacToeLogic.goal_valid_grid_has_no_more_than_one_move_diff",
      )
      |> Js.Promise.then_(
           fun
           | Belt.Result.Ok((Imandra_client.Verify.Proved, _)) =>
             Js.Promise.resolve(pass)
           | Belt.Result.Ok((Imandra_client.Verify.Unknown(_), _)) =>
             Js.Promise.resolve(fail("unknown"))
           | Belt.Result.Ok((Imandra_client.Verify.Refuted(_), _)) =>
             Js.Promise.resolve(fail("refuted"))
           | Belt.Result.Error((e, _)) => Js.Promise.resolve(fail(e)),
         );
    });
  });

let () =
  afterAllAsync(finish =>
    switch (runningImandraProcess^) {
    | Some(ip) =>
      Imandra_client.stop(ip)
      |> Js.Promise.then_(() => {
           runningImandraProcess := None;
           finish();
           Js.Promise.resolve();
         })
      |> ignore
    | None => finish()
    }
  );
