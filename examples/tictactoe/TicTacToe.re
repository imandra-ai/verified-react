open Css;

type raw_state = {
  game: TicTacToeLogic.game_state,
  status: TicTacToeLogic.game_status,
};

type state = {
  state: raw_state,
  passedState: option(raw_state),
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
let make = (~onGameFinished, ~state, _children) => {
  /* spread the other default fields of component here and override a few */
  ...component,
  initialState: () =>
    switch (state) {
    | None => {state: initialState, passedState: None}
    | Some(s) => {state: s, passedState: Some(s)}
    },
  willReceiveProps: self =>
    switch (state) {
    | Some(s) => {state: s, passedState: state}
    | _ => self.state
    },
  /* State transitions */
  reducer: (action, state) =>
    switch (action) {
    | Restart => ReasonReact.Update({...state, state: initialState})
    | Move(move) =>
      let (next, status) = TicTacToeLogic.play(state.state.game, move);
      let newState = {
        ...state,
        state: {
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
      switch (self.state.state.status) {
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
      switch (self.state.state.status) {
      | InvalidMove(m) when b == m =>
        merge([base, style([border(px(1), solid, red)])])
      | _ => base
      };
    };
    let overlay =
      switch (self.state.state.status) {
      | Tied => Some("=")
      | Won(X) => Some("X")
      | Won(O) => Some("O")
      | _ => None
      };
    let rowCss = style([display(flexBox), flexDirection(row)]);
    let elems =
      <div>
        <div className=rowCss>
          <button
            className={buttonCss(A)}
            onClick={_event => self.send(Move(A))}
            disabled>
            {ReasonReact.string(label(self.state.state.game.grid.a))}
          </button>
          <button
            className={buttonCss(B)}
            onClick={_event => self.send(Move(B))}
            disabled>
            {ReasonReact.string(label(self.state.state.game.grid.b))}
          </button>
          <button
            className={buttonCss(C)}
            onClick={_event => self.send(Move(C))}
            disabled>
            {ReasonReact.string(label(self.state.state.game.grid.c))}
          </button>
        </div>
        <div className=rowCss>
          <button
            className={buttonCss(D)}
            onClick={_event => self.send(Move(D))}
            disabled>
            {ReasonReact.string(label(self.state.state.game.grid.d))}
          </button>
          <button
            className={buttonCss(E)}
            onClick={_event => self.send(Move(E))}
            disabled>
            {ReasonReact.string(label(self.state.state.game.grid.e))}
          </button>
          <button
            className={buttonCss(F)}
            onClick={_event => self.send(Move(F))}
            disabled>
            {ReasonReact.string(label(self.state.state.game.grid.f))}
          </button>
        </div>
        <div className=rowCss>
          <button
            className={buttonCss(G)}
            onClick={_event => self.send(Move(G))}
            disabled>
            {ReasonReact.string(label(self.state.state.game.grid.g))}
          </button>
          <button
            className={buttonCss(H)}
            onClick={_event => self.send(Move(H))}
            disabled>
            {ReasonReact.string(label(self.state.state.game.grid.h))}
          </button>
          <button
            className={buttonCss(I)}
            onClick={_event => self.send(Move(I))}
            disabled>
            {ReasonReact.string(label(self.state.state.game.grid.i))}
          </button>
        </div>
      </div>;
    let sub =
      switch (overlay) {
      | None => [|elems|]
      | Some(overlayText) => [|
          elems,
          <div
            className={
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
                background(hex("#FBFBFB")),
                color(hex("#3276B5")),
                userSelect(none),
                cursor(`pointer),
              ])
            }
            onClick=(_event => self.send(Restart))>
            <div
              className={
                style([
                  display(flexBox),
                  flexDirection(column),
                  justifyContent(spaceAround),
                ])
              }>
              {ReasonReact.string(overlayText)}
            </div>
          </div>,
        |]
      };
    <div className={style([position(relative), display(flexBox)])}>
      ...sub
    </div>;
  },
};
