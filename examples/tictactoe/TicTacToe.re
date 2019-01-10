open Css;

type logic_state = {
  game: TicTacToeLogic.game_state,
  status: TicTacToeLogic.game_status,
};

type state = {
  customInitialLogic: option(logic_state),
  logic: logic_state,
};

type action =
  | Move(TicTacToeLogic.move)
  | Restart;

/* Component template declaration.
   Needs to be **after** state and action declarations! */
let component = ReasonReact.reducerComponent("Example");

let defaultInitialState = {
  game: TicTacToeLogic.initial_game,
  status: TicTacToeLogic.status(TicTacToeLogic.initial_game),
};

/* greeting and children are props. `children` isn't used, therefore ignored.
   We ignore it by prepending it with an underscore */
let make = (~customInitialLogic, ~onGameFinished, _children) => {
  /* spread the other default fields of component here and override a few */
  ...component,
  initialState: () =>
    switch (customInitialLogic) {
    | None => {logic: defaultInitialState, customInitialLogic: None}
    | Some(il) => {logic: il, customInitialLogic: Some(il)}
    },
  willReceiveProps: self =>
    switch (customInitialLogic, self.state.customInitialLogic) {
    | (Some(l1) as a, b) when a != b => {
        logic: l1,
        customInitialLogic: Some(l1),
      }
    | _ => self.state
    },
  /* State transitions */
  reducer: (action, state) =>
    switch (action) {
    | Restart =>
      ReasonReact.Update({
        ...state,
        logic:
          switch (customInitialLogic) {
          | None => defaultInitialState
          | Some(s) => s
          },
      })
    | Move(move) =>
      let (next, status) = TicTacToeLogic.play(state.logic.game, move);
      let newState = {
        ...state,
        logic: {
          status,
          game: next,
        },
      };
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
      switch (self.state.logic.status) {
      | InProgress => false
      | InvalidMove(_) => false
      | _ => true
      };
    let buttonCss = b => {
      let base =
        style([
          display(block),
          width(px(78)),
          height(px(78)),
          margin(px(3)),
        ]);
      switch (self.state.logic.status) {
      | InvalidMove(m) when b == m =>
        merge([base, style([border(px(1), solid, red)])])
      | _ => base
      };
    };
    let overlay =
      switch (self.state.logic.status) {
      | Tied => Some("=")
      | Won(X) => Some("X")
      | Won(O) => Some("O")
      | _ => None
      };
    let rowCss = style([display(flexBox), flexDirection(row)]);
    let elems =
      <div className=(style([display(flexBox), flexDirection(row)]))>
        <div>
          <div className=rowCss>
            <button
              className=(buttonCss(A))
              onClick=(_event => self.send(Move(A)))
              disabled>
              (ReasonReact.string(label(self.state.logic.game.grid.a)))
            </button>
            <button
              className=(buttonCss(B))
              onClick=(_event => self.send(Move(B)))
              disabled>
              (ReasonReact.string(label(self.state.logic.game.grid.b)))
            </button>
            <button
              className=(buttonCss(C))
              onClick=(_event => self.send(Move(C)))
              disabled>
              (ReasonReact.string(label(self.state.logic.game.grid.c)))
            </button>
          </div>
          <div className=rowCss>
            <button
              className=(buttonCss(D))
              onClick=(_event => self.send(Move(D)))
              disabled>
              (ReasonReact.string(label(self.state.logic.game.grid.d)))
            </button>
            <button
              className=(buttonCss(E))
              onClick=(_event => self.send(Move(E)))
              disabled>
              (ReasonReact.string(label(self.state.logic.game.grid.e)))
            </button>
            <button
              className=(buttonCss(F))
              onClick=(_event => self.send(Move(F)))
              disabled>
              (ReasonReact.string(label(self.state.logic.game.grid.f)))
            </button>
          </div>
          <div className=rowCss>
            <button
              className=(buttonCss(G))
              onClick=(_event => self.send(Move(G)))
              disabled>
              (ReasonReact.string(label(self.state.logic.game.grid.g)))
            </button>
            <button
              className=(buttonCss(H))
              onClick=(_event => self.send(Move(H)))
              disabled>
              (ReasonReact.string(label(self.state.logic.game.grid.h)))
            </button>
            <button
              className=(buttonCss(I))
              onClick=(_event => self.send(Move(I)))
              disabled>
              (ReasonReact.string(label(self.state.logic.game.grid.i)))
            </button>
          </div>
        </div>
        <div
          className=(
            style([
              display(flexBox),
              flexDirection(column),
              alignItems(center),
              marginLeft(px(10)),
            ])
          )>
          <div className=(style([marginBottom(px(10))]))>
            (ReasonReact.string("Next player:"))
          </div>
          <div
            className=(
              style([
                padding(px(10)),
                paddingTop(px(25)),
                paddingBottom(px(25)),
                minWidth(px(50)),
                backgroundColor(hex("F5FAFE")),
                textAlign(center),
              ])
            )>
            (
              ReasonReact.string(
                {
                  let next_player =
                    switch (self.state.logic.game.last_player) {
                    | None => TicTacToeLogic.initial_player
                    | Some(p) => TicTacToeLogic.other_player(p)
                    };
                  switch (next_player) {
                  | X => "X"
                  | O => "O"
                  };
                },
              )
            )
          </div>
        </div>
      </div>;
    let sub =
      switch (overlay) {
      | None => [|elems|]
      | Some(overlayText) => [|
          elems,
          <div
            className=(
              style([
                position(absolute),
                top(px(0)),
                left(px(0)),
                width(pct(100.)),
                height(pct(100.)),
                textAlign(center),
                fontSize(px(150)),
                display(flexBox),
                flexDirection(row),
                justifyContent(spaceAround),
                userSelect(none),
                cursor(`pointer),
              ])
            )
            onClick=(_event => self.send(Restart))>
            <div
              className=(
                style([
                  display(flexBox),
                  flexDirection(column),
                  justifyContent(spaceAround),
                ])
              )>
              (ReasonReact.string(overlayText))
            </div>
          </div>,
        |]
      };
    <div className=(style([position(relative), display(flexBox)]))>
      ...sub
    </div>;
  },
};
