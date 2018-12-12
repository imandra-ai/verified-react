open Jest;

let runningImandraProcess = ref(None);

let module_name = "TicTacToeLogic";

let () =
  beforeAllPromise(~timeout=20000, () =>
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
    beforeAllPromise(
      ~timeout=10000,
      () => {
        let ip = runningImandraProcess^ |> Belt.Option.getExn;
        let model_path =
          Node.Path.join([|
            [%raw "__dirname"],
            "..",
            Printf.sprintf("%s.ire", module_name),
          |]);
        Imandra_client.Eval.by_src(
          ip,
          ~src=Printf.sprintf("#mod_use \"%s\"", model_path),
        )
        |> Js.Promise.then_(
             fun
             | Belt.Result.Ok(_) => Js.Promise.resolve(pass)
             | Belt.Result.Error((e, _)) => {
                 Js.Console.error(e);
                 Js.Promise.reject(
                   Failure(Printf.sprintf("error from imandra: %s", e)),
                 );
               },
           );
      },
    );
    testPromise(
      ~timeout=10000,
      "verify move difference",
      () => {
        let ip = runningImandraProcess^ |> Belt.Option.getExn;
        let function_name =
          Imandra_client.function_name(
            TicTacToeLogic.goal_valid_grid_has_no_more_than_one_move_diff,
          );
        let name = Printf.sprintf("%s.%s", module_name, function_name);
        Imandra_client.Verify.by_name(ip, ~name)
        |> Js.Promise.then_(
             fun
             | Belt.Result.Ok((Imandra_client.Verify.Proved, _)) =>
               Js.Promise.resolve(pass)
             | Belt.Result.Ok(o) => {
                 Js.Console.error(o);
                 Js.Promise.reject(Failure("unexpected result"));
               }
             | Belt.Result.Error((e, _)) => {
                 Js.Console.error(e);
                 Js.Promise.reject(
                   Failure(Printf.sprintf("error from imandra: %s", e)),
                 );
               },
           );
      },
    );
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
