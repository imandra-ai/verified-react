open Css;

type action =
  | Noop
  | Initialised
  | QueryInstance(string)
  | InstanceReturned(string)
  | Error(string);

type state = {
  initialised: bool,
  loadingInstance: bool,
  instance: option(string),
  error: option(string),
};

let component = ReasonReact.reducerComponent("InstanceBrowser");

let serverInfo: Imandra_client.ServerInfo.t = {
  port: 3000,
  baseUrl: "http://localhost:3000",
};

let make = (~serverInfo, ~setupScriptPath, children) => {
  ...component,
  initialState: () => {
    initialised: false,
    instance: None,
    loadingInstance: false,
    error: None,
  },
  didMount: self => {
    let _p =
      Imandra_client.Eval.bySrc(
        ~syntax=Imandra_client.Syntax.Reason,
        ~src=Printf.sprintf("#use \"%s\"", setupScriptPath),
        serverInfo,
      )
      |> Js.Promise.then_(_v => {
           self.send(Initialised);
           Js.Promise.resolve();
         });
    ();
  },
  reducer: (action, s: state) =>
    switch (action) {
    | Noop => ReasonReact.Update(s)
    | Error(e) => ReasonReact.Update({...s, error: Some(e)})
    | Initialised => ReasonReact.Update({...s, initialised: true})
    | QueryInstance(queryStr) =>
      ReasonReact.UpdateWithSideEffects(
        {...s, loadingInstance: true},
        (
          self => {
            let _p =
              Imandra_client.Instance.bySrc(
                ~syntax=Imandra_client.Syntax.Reason,
                ~src=Printf.sprintf("x => %s", queryStr),
                ~instancePrinter={
                  name: "game_state_to_json_pp",
                  cx_var_name: "x",
                },
                serverInfo,
              )
              |> Js.Promise.then_(v => {
                   switch (v) {
                   | Belt.Result.Ok((Imandra_client.Instance.Sat(s), _)) =>
                     self.send(
                       InstanceReturned(
                         s.instance.printed |> Belt.Option.getExn,
                       ),
                     )
                   | _ => self.send(Error("error calling instance"))
                   };
                   Js.Promise.resolve();
                 });
            ();
          }
        ),
      )
    | InstanceReturned(instance) =>
      ReasonReact.Update({
        ...s,
        loadingInstance: false,
        instance: Some(instance),
      })
    },
  render: self =>
    <div className=(style([display(flexBox), flexDirection(row)]))>
      <div> ...children </div>
      <div className=(style([display(flexBox), flexDirection(column)]))>
        <div> (ReasonReact.string("Instance query:")) </div>
        <textarea
          onChange=(
            event =>
              self.send(QueryInstance(ReactEvent.Form.target(event)##value))
          )
        />
      </div>
    </div>,
};
