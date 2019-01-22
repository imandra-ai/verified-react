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
  let component = ReasonReact.statelessComponent("App");
  let make = _children => {
    ...component,
    render: _self =>
      <div
        className=(
          style([
            display(flexBox),
            flexDirection(column),
            alignItems(center),
          ])
        )>
        <h1> (ReasonReact.string("Tic Tac Toe")) </h1>
        <InstanceBrowser
          serverInfo=Imandra_client.Server_info.{
                       port: 3000,
                       base_url: "http://localhost:3000",
                     }
          setupScriptPath="examples/tictactoe/TicTacToe_setup.ire"
          instanceType="game_state"
          instancePrinterFn="game_state_to_json_pp"
          examples=[
            "is_valid_game(x)",
            "x.grid.c == Some(X)",
            "status(x) == Won(X)",
            "status(x) == Won(O) && is_valid_game(x)",
            "is_valid_game(x) && x.last_player == Some(O)",
            "is_valid_game(x) && status(x) == Tied",
            "true",
          ]
          body=(
            instanceJson => {
              let instance =
                switch (instanceJson) {
                | None => None
                | Some(j) =>
                  let game = game_state_of_json(j);
                  Some({
                    TicTacToe.game,
                    TicTacToe.status: TicTacToeLogic.status(game),
                  });
                };
              <div
                className=(
                  style([
                    display(flexBox),
                    flexDirection(column),
                    fontSize(rem(1.2)),
                    marginTop(px(10)),
                  ])
                )>
                <TicTacToe
                  onGameFinished=(() => ())
                  customInitialLogicState=instance
                />
              </div>;
            }
          )
        />
      </div>,
  };
};

ReactDOMRe.renderToElementWithId(<App />, "app");
