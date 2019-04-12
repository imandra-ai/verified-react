open Css;

type logic_state = {
  game: TicTacToe_Logic.game_state,
  status: TicTacToe_Logic.game_status,
};

type state = {
  customInitialLogicState: option(logic_state),
  logic: logic_state,
};

type action =
  | Move(TicTacToe_Logic.move)
  | Restart;

let defaultInitialState = {
  game: TicTacToe_Logic.initial_game,
  status: TicTacToe_Logic.status(TicTacToe_Logic.initial_game),
};

/*graphic symbols for the game*/
let svgE = <img src=[%raw "require('../../img/sign-e.svg')"] />;
let svgX = <img src=[%raw "require('../../img/sign-x.svg')"] />;
let svgO = <img src=[%raw "require('../../img/sign-o.svg')"] />;

let init = (~customInitialLogicState) =>
  switch (customInitialLogicState) {
  | None => {logic: defaultInitialState, customInitialLogicState: None}
  | Some(il) => {logic: il, customInitialLogicState: Some(il)}
  };

/* State transitions */
let reducer = (~customInitialLogicState, state, action) =>
  switch (action) {
  | Restart => {
      ...state,
      logic:
        switch (customInitialLogicState) {
        | None => defaultInitialState
        | Some(s) => s
        },
    }
  | Move(move) =>
    let (next, status) = TicTacToe_Logic.play(state.logic.game, move);
    let newState = {
      ...state,
      logic: {
        status,
        game: next,
      },
    };
    newState;
  };

[@react.component]
let make = (~customInitialLogicState, ~onGameFinished): React.element => {
  /* spread the other default fields of component here and override a few */
  let (state, dispatch) =
    React.useReducer(
      reducer(~customInitialLogicState),
      init(~customInitialLogicState),
    );
  /* ???
     willReceiveProps: self =>
         switch (customInitialLogicState, self.state.customInitialLogicState) {
       | (Some(l1) as a, b) when a != b => {
           logic: l1,
           customInitialLogicState: Some(l1),
         }
       | _ => self.state
       },
          */
  let () =
    React.useEffect1(
      () =>
        switch (state.logic.status) {
        | Won(_)
        | Tied => Some((() => onGameFinished()))
        | InProgress
        | InvalidMove(_) => None
        },
      [|state.logic.status|],
    );
  let label = s =>
    switch (s) {
    | None => <div />
    | Some(TicTacToe_Logic.X) => svgX
    | Some(TicTacToe_Logic.O) => svgO
    };
  let disabled =
    switch (state.logic.status) {
    | InProgress => false
    | InvalidMove(_) => false
    | _ => true
    };
  let buttonCss = b => {
    let base =
      style([
        display(block),
        width(px(88)),
        height(px(88)),
        margin(px(3)),
      ]);
    switch (state.logic.status) {
    | InvalidMove(m) when b == m =>
      merge([base, style([border(px(1), solid, red)])])
    | _ => base
    };
  };
  let overlay =
    switch (state.logic.status) {
    | Tied => Some(svgE)
    | Won(X) => Some(svgX)
    | Won(O) => Some(svgO)
    | _ => None
    };
  let rowCss = style([display(flexBox), flexDirection(row)]);
  let elems =
    <div className="tictactoe__instances">
      <div className=rowCss>
        <button
          className={"tictactoe__button " ++ buttonCss(A)}
          onClick={_event => dispatch(Move(A))}
          disabled>
          {label(state.logic.game.grid.a)}
        </button>
        <button
          className={"tictactoe__button " ++ buttonCss(B)}
          onClick={_event => dispatch(Move(B))}
          disabled>
          {label(state.logic.game.grid.b)}
        </button>
        <button
          className={"tictactoe__button " ++ buttonCss(C)}
          onClick={_event => dispatch(Move(C))}
          disabled>
          {label(state.logic.game.grid.c)}
        </button>
      </div>
      <div className=rowCss>
        <button
          className={"tictactoe__button " ++ buttonCss(D)}
          onClick={_event => dispatch(Move(D))}
          disabled>
          {label(state.logic.game.grid.d)}
        </button>
        <button
          className={"tictactoe__button " ++ buttonCss(E)}
          onClick={_event => dispatch(Move(E))}
          disabled>
          {label(state.logic.game.grid.e)}
        </button>
        <button
          className={"tictactoe__button " ++ buttonCss(F)}
          onClick={_event => dispatch(Move(F))}
          disabled>
          {label(state.logic.game.grid.f)}
        </button>
      </div>
      <div className=rowCss>
        <button
          className={"tictactoe__button " ++ buttonCss(G)}
          onClick={_event => dispatch(Move(G))}
          disabled>
          {label(state.logic.game.grid.g)}
        </button>
        <button
          className={"tictactoe__button " ++ buttonCss(H)}
          onClick={_event => dispatch(Move(H))}
          disabled>
          {label(state.logic.game.grid.h)}
        </button>
        <button
          className={"tictactoe__button " ++ buttonCss(I)}
          onClick={_event => dispatch(Move(I))}
          disabled>
          {label(state.logic.game.grid.i)}
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
              userSelect(none),
              cursor(`pointer),
            ])
          }
          onClick=(_event => dispatch(Restart))>
          <div
            className={
              "tictactoe__result "
              ++ style([
                   display(flexBox),
                   flexDirection(column),
                   justifyContent(spaceAround),
                 ])
            }>
            overlayText
          </div>
        </div>,
      |]
    };
  <div>
    <div
      className={
        style([
          position(relative),
          display(flexBox),
          justifyContent(spaceAround),
        ])
      }>
      {React.array(sub)}
    </div>
    <div
      className={
        style([
          display(flexBox),
          flexDirection(column),
          alignItems(center),
          marginLeft(px(10)),
        ])
      }>
      <div className={style([marginBottom(px(10))])}>
        {React.string("Next player:")}
      </div>
      <div className="tictactoe__button tictactoe__button--silent ">
        {
          let next_player =
            switch (state.logic.game.last_player) {
            | None => TicTacToe_Logic.initial_player
            | Some(p) => TicTacToe_Logic.other_player(p)
            };
          switch (next_player) {
          | X => svgX
          | O => svgO
          };
        }
      </div>
    </div>
  </div>;
};
