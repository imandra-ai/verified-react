open Css;
type action =
  | Noop;
type state =
  | ();
let component = ReasonReact.reducerComponent("InstanceBrowser");
let make = (~setupScriptPath, children) => {
  ...component,
  initialState: () => (),
  didMount: _c => {
    let serverInfo: Imandra_client.ServerInfo.t = {
      port: 3000,
      baseUrl: "http://localhost:3000",
    };
    print_endline("calling eval");
    let _p =
      Imandra_client.Eval.bySrc(
        ~syntax=Imandra_client.Syntax.Reason,
        ~src=Printf.sprintf("#use \"%s\"", setupScriptPath),
        serverInfo,
      )
      |> Js.Promise.then_(_ => {
           print_endline("calling instance");
           Imandra_client.Instance.bySrc(
             ~syntax=Imandra_client.Syntax.Reason,
             ~src="x => x.last_player == Some(X)",
             ~instancePrinter={ name: "game_state_to_json_pp", cx_var_name: "x" },
             serverInfo,
           );
         });
    ();
  },
  reducer: (action, s: state) =>
    switch (action) {
    | Noop => ReasonReact.Update(s)
    },
  render: _self =>
    <div className={style([display(flexBox), flexDirection(row)])}>
      <div> ...children </div>
      <div className={style([display(flexBox), flexDirection(column)])}>
        <div> {ReasonReact.string("Instance query:")} </div>
        <textarea />
      </div>
    </div>,
};
