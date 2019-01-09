open Css;

let player_of_json = json => {
  let s = Json.Decode.optional(Json.Decode.string, json);
  switch (s) {
  | Some("X") => Some(TicTacToeLogic.X)
  | Some("O") => Some(TicTacToeLogic.O)
  | _ => None
  };
};

let grid_of_json = json => {
  let a = Json.Decode.field("a", player_of_json, json);
  let b = Json.Decode.field("b", player_of_json, json);
  let c = Json.Decode.field("c", player_of_json, json);
  let d = Json.Decode.field("d", player_of_json, json);
  let e = Json.Decode.field("e", player_of_json, json);
  let f = Json.Decode.field("f", player_of_json, json);
  let g = Json.Decode.field("g", player_of_json, json);
  let h = Json.Decode.field("h", player_of_json, json);
  let i = Json.Decode.field("i", player_of_json, json);
  TicTacToeLogic.{a, b, c, d, e, f, g, h, i};
};

let game_state_of_json = json => {
  let last_player = Json.Decode.field("last_player", player_of_json, json);
  let grid = Json.Decode.field("grid", grid_of_json, json);
  TicTacToeLogic.{grid, last_player};
};

module App = {
  type status =
    | InProgress
    | Finished;
  type state = {
    status,
    isQuerying: bool,
  };
  type action =
    | Finish
    | Restart
    | SetQuerying(bool);
  let component = ReasonReact.reducerComponent("App");
  let make = _children => {
    ...component,
    initialState: () => {status: InProgress, isQuerying: false},
    reducer: (action, state) =>
      switch (action) {
      | Finish => ReasonReact.Update({...state, status: Finished})
      | Restart => ReasonReact.Update({...state, status: InProgress})
      | SetQuerying(v) => ReasonReact.Update({...state, isQuerying: v})
      },
    render: self => {
      let contents = json => {
        switch (json) {
        | None => print_endline("none")
        | Some(j) => print_endline(Js.Json.stringify(j))
        };
        let state =
          switch (json) {
          | None => None
          | Some(j) =>
            print_endline("in here");
            Some({
              TicTacToe.game: game_state_of_json(j),
              TicTacToe.status: InProgress,
            });
          };
        <div
          className={
            style([
              display(flexBox),
              flexDirection(column),
              fontSize(rem(1.2)),
              marginTop(px(10)),
            ])
          }>
          <TicTacToe onGameFinished={() => self.send(Finish)} state />
          <div>
            <input
              type_="checkbox"
              checked={self.state.isQuerying}
              onChange={
                event =>
                  self.send(
                    SetQuerying(ReactEvent.Form.target(event)##checked),
                  )
              }
            />
            <label className={style([paddingLeft(px(5))])}>
              {ReasonReact.string("Query instances")}
            </label>
          </div>
        </div>;
      };
      <div
        className={
          style([
            display(flexBox),
            flexDirection(column),
            alignItems(center),
          ])
        }>
        <h1> {ReasonReact.string("Tic Tac Toe")} </h1>
        {
          if (self.state.isQuerying) {
            <InstanceBrowser
              serverInfo=Imandra_client.ServerInfo.{
                port: 3000,
                baseUrl: "http://localhost:3000",
              }
              setupScriptPath="examples/tictactoe/Setup.ire"
              body=contents
            />;
          } else {
            contents(None);
          }
        }
      </div>;
    },
  };
};

ReactDOMRe.renderToElementWithId(<App />, "app");
