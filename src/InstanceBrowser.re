open Css;
type action =
  | Noop;
type state =
  | ();
let component = ReasonReact.reducerComponent("InstanceBrowser");
let make = (~srcPath, children) => {
  ...component,
  initialState: () => (),
  didMount: _c => {
    let serverInfo: Imandra_client.ServerInfo.t = {
      port: 3000,
      baseUrl: "http://localhost:3000",
    };
    print_endline("calling eval");
    let _p =
      Imandra_client.Eval.by_src(
        ~syntax=Imandra_client.Syntax.Reason,
        ~src=Printf.sprintf("#use \"%s\"", srcPath),
        serverInfo,
      );
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
