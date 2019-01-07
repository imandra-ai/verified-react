open Css;

module App = {
  type status =
    | InProgress
    | Finished;
  type state = {
    status,
    isDebug: bool,
  };
  type action =
    | Finish
    | Restart
    | SetDebug(bool);
  let component = ReasonReact.reducerComponent("App");
  let make = _children => {
    ...component,
    initialState: () => {status: InProgress, isDebug: false},
    reducer: (action, state) =>
      switch (action) {
      | Finish => ReasonReact.Update({...state, status: Finished})
      | Restart => ReasonReact.Update({...state, status: InProgress})
      | SetDebug(v) => ReasonReact.Update({...state, isDebug: v})
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
        {
          if (self.state.isDebug) {
            <InstanceBrowser>
              <TicTacToe onGameFinished={() => self.send(Finish)} />
            </InstanceBrowser>;
          } else {
            <TicTacToe onGameFinished={() => self.send(Finish)} />;
          }
        }
        <input
          type_="checkbox"
          checked={self.state.isDebug}
          onChange={
            event =>
              self.send(SetDebug(ReactEvent.Form.target(event)##checked))
          }
        />
      </div>,
  };
};

ReactDOMRe.renderToElementWithId(<App />, "app");
