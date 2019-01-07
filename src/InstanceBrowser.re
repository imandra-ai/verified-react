open Css;
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
  render: _self =>
    <div className={style([display(flexBox), flexDirection(row)])}>
      <div> ...children </div>
      <div className={style([display(flexBox), flexDirection(column)])}>
        <div> {ReasonReact.string("Instance query:")} </div>
        <textarea />
      </div>
    </div>,
};
