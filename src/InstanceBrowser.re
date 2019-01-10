open Css;

type action =
  | Noop
  | InitialiseError(string)
  | Initialised
  | QueryInstance(string)
  | InstanceReturned(string)
  | Error(string);

type initState =
  | Loading
  | Error
  | Loaded;

type state = {
  init: initState,
  loadingInstance: bool,
  instance: option(Js.Json.t),
  query: string,
  error: option(string),
};

let component = ReasonReact.reducerComponent("InstanceBrowser");

let serverInfo: Imandra_client.ServerInfo.t = {
  port: 3000,
  baseUrl: "http://localhost:3000",
};

let make = (~serverInfo, ~setupScriptPath, ~body, _children) => {
  ...component,
  initialState: () => {
    init: Loading,
    instance: None,
    loadingInstance: false,
    query: "",
    error: None,
  },
  didMount: self => {
    let _p =
      Imandra_client.Eval.bySrc(
        ~syntax=Imandra_client.Syntax.Reason,
        ~src=Printf.sprintf("#use \"%s\"", setupScriptPath),
        serverInfo,
      )
      |> Js.Promise.then_(v => {
           switch (v) {
           | Belt.Result.Ok(_) => self.send(Initialised)
           | Belt.Result.Error((e, _)) => self.send(InitialiseError(e))
           };
           Js.Promise.resolve();
         });
    ();
  },
  reducer: (action, s: state) =>
    switch (action) {
    | Noop => ReasonReact.Update(s)
    | Error(e) => ReasonReact.Update({...s, error: Some(e)})
    | Initialised => ReasonReact.Update({...s, init: Loaded})
    | InitialiseError(e) =>
      Js.Console.error(e);
      ReasonReact.Update({...s, init: Error});
    | QueryInstance(queryStr) =>
      ReasonReact.UpdateWithSideEffects(
        {...s, loadingInstance: true, query: queryStr},
        (
          self => {
            let _p =
              Imandra_client.Instance.bySrc(
                ~syntax=Imandra_client.Syntax.Reason,
                ~src=Printf.sprintf("(x : game_state) => %s", queryStr),
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
        instance: Json.parse(instance),
      })
    },
  render: self => {
    let example = s =>
      <button onClick=(_e => self.send(QueryInstance(s)))>
        <pre> (ReasonReact.string(s)) </pre>
      </button>;
    <div
      className=(
        style([display(flexBox), flexDirection(column), fontSize(px(14))])
      )>
      <div> (body(self.state.instance)) </div>
      <div
        className=(
          style([
            display(flexBox),
            flexDirection(column),
            marginTop(px(20)),
          ])
        )>
        <div
          className=(
            style([
              fontSize(px(16)),
              maxWidth(px(400)),
              marginBottom(px(20)),
            ])
          )>
          (ReasonReact.string("Generate instances based purely on "))
          <a
            href="https://github.com/AestheticIntegration/verified-react/blob/master/examples/tictactoe/TicTacToeLogic.ire">
            (ReasonReact.string("the source code of the game logic"))
          </a>
          (ReasonReact.string(" which has been loaded into Imandra."))
        </div>
        <div> (ReasonReact.string("Enter a ReasonML instance query:")) </div>
        <div
          className=(
            style([display(flexBox), justifyContent(spaceBetween)])
          )>
          <pre> (ReasonReact.string("x : game_state  =>")) </pre>
          <div className=(style([display(flexBox), alignItems(center)]))>
            <div className=(style([fontSize(px(8)), marginRight(px(5))]))>
              (ReasonReact.string("imandra-http-server connection"))
            </div>
            <div
              className=(
                style([
                  borderRadius(pct(50.)),
                  height(px(20)),
                  width(px(20)),
                  backgroundColor(
                    switch (self.state.init) {
                    | Loading => grey
                    | Loaded => green
                    | Error => red
                    },
                  ),
                ])
              )
            />
          </div>
        </div>
        <div className=(style([display(flexBox), flexDirection(column)]))>
          <textarea
            className=(style([height(px(100)), padding(px(5))]))
            value=self.state.query
            onChange=(
              event =>
                self.send(
                  QueryInstance(ReactEvent.Form.target(event)##value),
                )
            )
          />
        </div>
        <div className=(style([marginTop(px(20))]))>
          (ReasonReact.string("or try these examples:"))
        </div>
        <ul>
          <li> (example("is_valid_game(x)")) </li>
          <li> (example("is_winning(x, X)")) </li>
          <li> (example("is_winning(x, O) && is_valid_game(x)")) </li>
          <li> (example("is_valid_game(x) && x.last_player == Some(O)")) </li>
          <li> (example("true")) </li>
        </ul>
      </div>
    </div>;
  },
};
