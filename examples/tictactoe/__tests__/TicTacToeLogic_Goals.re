open Jest;

let serverInfo = ref(None);

let moduleName = "TicTacToeLogic";

module I = Imandra_client;

let () =
  beforeAllPromise(() => {
    let si = Imandra_client.ServerInfo.fromFile() |> Belt.Result.getExn;
    serverInfo := Some(si);
    Imandra_client.reset(si);
  });

let syntax = I.Api.Reason;

let ppErr = e => Format.asprintf("%a", I.Error.pp, e);

let () =
  describe("tic tac toe model", () => {
    beforeAllPromise(() => {
      let ip = serverInfo^ |> Belt.Option.getExn;
      let model_path =
        Node.Path.join([|
          [%raw "__dirname"],
          "..",
          Printf.sprintf("%s.ire", moduleName),
        |]);
      Imandra_client.Eval.bySrc(
        ip,
        ~syntax,
        ~src=Printf.sprintf("#mod_use \"%s\"", model_path),
      )
      |> Js.Promise.then_(
           fun
           | Belt.Result.Ok(_) => Js.Promise.resolve(pass)
           | Belt.Result.Error(e) => {
               Js.Console.error(ppErr(e));
               Js.Promise.reject(Failure(ppErr(e)));
             },
         );
    });
    testPromise("verify move difference", () => {
      let ip = serverInfo^ |> Belt.Option.getExn;
      let functionName =
        Imandra_client.function_name(
          TicTacToeLogic.goal_valid_grid_has_no_more_than_one_move_diff,
        );
      let name = Printf.sprintf("%s.%s", moduleName, functionName);
      Imandra_client.Verify.byName(ip, ~name)
      |> Js.Promise.then_(
           fun
           | Belt.Result.Ok(I.Api.Response.V_proved) =>
             Js.Promise.resolve(pass)
           | Belt.Result.Ok(o) => {
               Js.Console.error(o);
               Js.Promise.reject(Failure("unexpected result"));
             }
           | Belt.Result.Error(e) => {
               Js.Console.error(ppErr(e));
               Js.Promise.reject(Failure(ppErr(e)));
             },
         );
    });
    testPromise(
      ~timeout=300000,
      "verify game progression",
      () => {
        let ip = serverInfo^ |> Belt.Option.getExn;
        let functionName =
          Imandra_client.function_name(TicTacToeLogic.goal_game_progression);
        let name = Printf.sprintf("%s.%s", moduleName, functionName);
        Imandra_client.Verify.byName(ip, ~name)
        |> Js.Promise.then_(
             fun
             | Belt.Result.Ok(I.Api.Response.V_proved) =>
               Js.Promise.resolve(pass)
             | Belt.Result.Ok(o) => {
                 Js.Console.error(o);
                 Js.Promise.reject(Failure("unexpected result"));
               }
             | Belt.Result.Error(e) => {
                 Js.Console.error(ppErr(e));
                 Js.Promise.reject(Failure(ppErr(e)));
               },
           );
      },
    );
  });
