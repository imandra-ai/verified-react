type action =
  | Noop;
type state = {yo: string};
let component = ReasonReact.reducerComponent("InstanceBrowser");
let make = children => {
  ...component,
  initialState: () => {yo: "there"},
  reducer: (action, s: state) =>
    switch (action) {
    | Noop => ReasonReact.Update(s)
    },
  render: self =>
    <div>
      <h1> {ReasonReact.string("Inspecting!")} </h1>
      <div> ...children </div>
    </div>,
};
