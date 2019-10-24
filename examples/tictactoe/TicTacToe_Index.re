let player_of_json = json => {
  let s = Json.Decode.optional(Json.Decode.string, json);
  switch (s) {
  | Some("X") => Some(TicTacToe_Logic.X)
  | Some("O") => Some(TicTacToe_Logic.O)
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
  TicTacToe_Logic.{a, b, c, d, e, f, g, h, i};
};

let game_state_of_json = json => {
  let last_player = Json.Decode.field("last_player", player_of_json, json);
  let grid = Json.Decode.field("grid", grid_of_json, json);
  TicTacToe_Logic.{grid, last_player};
};

module App = {
  let component = ReasonReact.statelessComponent("App");
  let make = _children => {
    ...component,
    render: _self =>
      <InstanceBrowser
        serverInfo=Imandra_client.Server_info.{url: "http://localhost:3000"}
        setupScriptPath="examples/tictactoe/TicTacToe_Setup.ire"
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
        body={instanceJson => {
          let instance =
            switch (instanceJson) {
            | None => None
            | Some(j) =>
              let game = game_state_of_json(j);
              Some({
                TicTacToe.game,
                TicTacToe.status: TicTacToe_Logic.status(game),
              });
            };

          <div className="instance">
            <h1 className="instance__title">
              {ReasonReact.string("Tic Tac Toe")}
            </h1>
            <section className="tictactoe">
              <TicTacToe
                onGameFinished={() => ()}
                customInitialLogicState=instance
              />
            </section>
            <div className="instance__description">
              <h2 className="instance__subtitle">
                {ReasonReact.string(
                   "Generate instances of TicTacToe based purely on ",
                 )}
                <a
                  className="instance__subtitle--link"
                  href="https://github.com/AestheticIntegration/verified-react/blob/master/examples/tictactoe/TicTacToe_Logic.ire">
                  {ReasonReact.string("the source code of the game logic")}
                </a>
                {ReasonReact.string(" which has been loaded into Imandra.")}
              </h2>
            </div>
          </div>;
        }}
      />,
  };
};

ReactDOMRe.renderToElementWithId(<App />, "app");
