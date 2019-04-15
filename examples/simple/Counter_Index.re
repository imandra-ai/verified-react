let s = ReasonReact.string;
module App = {
  let component = ReasonReact.reducerComponent("App");
  let make = _children => {
    ...component,
    initialState: _ => ({count: Z.of_int(0)}: Counter.model),
    reducer: (action, state) =>
      ReasonReact.Update(Counter.update(state, action)),
    render: self =>
      <div>
        {s("Count: " ++ Z.to_string(self.state.Counter.count))}
        <button onClick={_ => self.send(Counter.Increment)}>
          {s("Increment")}
        </button>
        <button onClick={_ => self.send(Counter.Decrement)}>
          {s("Decrement")}
        </button>
      </div>,
  };
};

ReactDOMRe.renderToElementWithId(<App />, "app");
