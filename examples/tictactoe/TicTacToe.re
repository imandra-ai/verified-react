[@bs.module "emotion"] external css : string => string = "css";

type state = {
  game: TicTacToeLogic.game_state,
  status: TicTacToeLogic.game_status,
};

type action =
  | Move(TicTacToeLogic.move)
  | Restart;

/* Component template declaration.
   Needs to be **after** state and action declarations! */
let component = ReasonReact.reducerComponent("Example");

let initialState = {
  game: TicTacToeLogic.initial_game,
  status: TicTacToeLogic.status(TicTacToeLogic.initial_game),
};

/* greeting and children are props. `children` isn't used, therefore ignored.
   We ignore it by prepending it with an underscore */
let make = (~onGameFinished, _children) => {
  /* spread the other default fields of component here and override a few */
  ...component,
  initialState: () => initialState,
  /* State transitions */
  reducer: (action, state) =>
    switch (action) {
    | Restart => ReasonReact.Update(initialState)
    | Move(move) =>
      let (next, status) = TicTacToeLogic.play(state.game, move);
      let newState = {status, game: next};
      switch (status) {
      | Won(_)
      | Tied =>
        ReasonReact.UpdateWithSideEffects(
          newState,
          (_self => onGameFinished()),
        )
      | InProgress
      | InvalidMove(_) => ReasonReact.Update(newState)
      };
    },
  render: self => {
    let label = s =>
      switch (s) {
      | None => ""
      | Some(TicTacToeLogic.X) => "X"
      | Some(TicTacToeLogic.O) => "O"
      };
    let disabled =
      switch (self.state.status) {
      | InProgress => false
      | InvalidMove(_) => false
      | _ => true
      };
    let buttonCss = b => {
      let base =
        css("display: block; width: 78px; height: 78px; margin: 3px;");
      switch (self.state.status) {
      | InvalidMove(m) when b == m =>
        base ++ " " ++ css("border: solid 1px red !important")
      | _ => base
      };
    };
    let overlay =
      switch (self.state.status) {
      | Tied => Some("=")
      | Won(X) => Some("X")
      | Won(O) => Some("O")
      | _ => None
      };
    let rowCss = css("display: flex; flex-direction: row");
    let elems =
      <div>
        <div className=rowCss>
          <button
            className=(buttonCss(A))
            onClick=(_event => self.send(Move(A)))
            disabled>
            (ReasonReact.string(label(self.state.game.grid.a)))
          </button>
          <button
            className=(buttonCss(B))
            onClick=(_event => self.send(Move(B)))
            disabled>
            (ReasonReact.string(label(self.state.game.grid.b)))
          </button>
          <button
            className=(buttonCss(C))
            onClick=(_event => self.send(Move(C)))
            disabled>
            (ReasonReact.string(label(self.state.game.grid.c)))
          </button>
        </div>
        <div className=rowCss>
          <button
            className=(buttonCss(D))
            onClick=(_event => self.send(Move(D)))
            disabled>
            (ReasonReact.string(label(self.state.game.grid.d)))
          </button>
          <button
            className=(buttonCss(E))
            onClick=(_event => self.send(Move(E)))
            disabled>
            (ReasonReact.string(label(self.state.game.grid.e)))
          </button>
          <button
            className=(buttonCss(F))
            onClick=(_event => self.send(Move(F)))
            disabled>
            (ReasonReact.string(label(self.state.game.grid.f)))
          </button>
        </div>
        <div className=rowCss>
          <button
            className=(buttonCss(G))
            onClick=(_event => self.send(Move(G)))
            disabled>
            (ReasonReact.string(label(self.state.game.grid.g)))
          </button>
          <button
            className=(buttonCss(H))
            onClick=(_event => self.send(Move(H)))
            disabled>
            (ReasonReact.string(label(self.state.game.grid.h)))
          </button>
          <button
            className=(buttonCss(I))
            onClick=(_event => self.send(Move(I)))
            disabled>
            (ReasonReact.string(label(self.state.game.grid.i)))
          </button>
        </div>
      </div>;
    let sub =
      switch (overlay) {
      | None => [|elems|]
      | Some(overlayText) => [|
          elems,
          <div
            className=(
              css(
                "position: absolute; top: 0; left: 0; width: 100%; height: 100%; text-align: center; font-size: 150px; display: flex; flex-direction: row; justify-content: space-around; background: #FBFBFB; color: #3276B5; user-select: none; cursor: pointer;",
              )
            )
            onClick=(_event => self.send(Restart))>
            <div
              className=(
                css(
                  "display: flex; flex-direction: column; justify-content: space-around;",
                )
              )>
              (ReasonReact.string(overlayText))
            </div>
          </div>,
        |]
      };
    <div className=(css("position: relative; display: flex;"))> ...sub </div>;
  },
};
