open Jest;

let serverInfo = ref(None);

let moduleName = "TodoMvcModel";

let () =
  beforeAllPromise(() => {
    let si = Imandra_client.ServerInfo.fromFile();
    serverInfo := Some(si);
    Imandra_client.reset(si);
  });

let syntax = Imandra_client.Syntax.Reason;

describe("todomvc model", () => {
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
         | Belt.Result.Error((e, _)) => {
             Js.Console.error(e);
             Js.Promise.reject(
               Failure(Printf.sprintf("error from imandra: %s", e)),
             );
           },
       );
  });
  testPromise("vg_add_todo", () => {
    let ip = serverInfo^ |> Belt.Option.getExn;
    let functionName = Imandra_client.function_name(TodoMvcModel.vg_add_todo);
    let name = Printf.sprintf("%s.%s", moduleName, functionName);
    Imandra_client.Verify.byName(ip, ~name)
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
