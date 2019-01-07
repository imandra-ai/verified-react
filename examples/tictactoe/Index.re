open Css;

module App = {
  type status =
    | InProgress
    | Finished;
  type state = {
    status,
    debugMode: bool,
  };
  type action =
    | Finish
    | Restart;
  let component = ReasonReact.reducerComponent("App");
  let make = _children => {
    ...component,
    initialState: () => {status: InProgress, debugMode: false},
    reducer: (action, state) =>
      switch (action) {
      | Finish => ReasonReact.Update({...state, status: Finished})
      | Restart => ReasonReact.Update({...state, status: InProgress})
      },
    render: self =>
      <div
        className={
          style([
            display(flexBox),
            flexDirection(column),
            alignItems(center),
          ])
        }>
        <h1> {ReasonReact.string("Tic Tac Toe")} </h1>
        <TicTacToe onGameFinished={() => self.send(Finish)} />
      </div>,
  };
};

ReactDOMRe.renderToElementWithId(<App />, "app");
