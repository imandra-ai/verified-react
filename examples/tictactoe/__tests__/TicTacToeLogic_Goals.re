open Jest;

let serverInfo = ref(None);

let module_name = "TicTacToeLogic";

let () =
  beforeAllPromise(() => {
    let si = Imandra_client.ServerInfo.from_file();
    serverInfo := Some(si);
    Imandra_client.reset(si);
  });

let syntax = Imandra_client.Syntax.Reason;

let () =
  describe("tic tac toe model", () => {
    beforeAllPromise(() => {
      let ip = serverInfo^ |> Belt.Option.getExn;
      let model_path =
        Node.Path.join([|
          [%raw "__dirname"],
          "..",
          Printf.sprintf("%s.ire", module_name),
        |]);
      Imandra_client.Eval.by_src(
        ip,
        ~syntax,
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
    });
    testPromise("verify move difference", () => {
      let ip = serverInfo^ |> Belt.Option.getExn;
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
    });
  });
