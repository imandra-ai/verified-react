open Css;

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
      let contents = state =>
        <div
          className=(
            style([
              display(flexBox),
              flexDirection(column),
              fontSize(rem(1.2)),
              marginTop(px(10)),
            ])
          )>
          <TicTacToe onGameFinished=(() => self.send(Finish)) state />
          <div>
            <input
              type_="checkbox"
              checked=self.state.isQuerying
              onChange=(
                event =>
                  self.send(
                    SetQuerying(ReactEvent.Form.target(event)##checked),
                  )
              )
            />
            <label className=(style([paddingLeft(px(5))]))>
              (ReasonReact.string("Query instances"))
            </label>
          </div>
        </div>;
      <div
        className=(
          style([
            display(flexBox),
            flexDirection(column),
            alignItems(center),
          ])
        )>
        <h1> (ReasonReact.string("Tic Tac Toe")) </h1>
        (
          if (self.state.isQuerying) {
            <InstanceBrowser
              serverInfo=Imandra_client.ServerInfo.{
                           port: 3000,
                           baseUrl: "http://localhost:3000",
                         }
              setupScriptPath="examples/tictactoe/Setup.ire">
              (contents(None))
            </InstanceBrowser>;
          } else {
            contents(None);
          }
        )
      </div>;
    },
  };
};

ReactDOMRe.renderToElementWithId(<App />, "app");
