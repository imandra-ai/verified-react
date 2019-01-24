let component = ReasonReact.statelessComponent("TodoFooter");

let make =
    (
      ~count,
      ~completedCount,
      ~nowShowing,
      ~onClearCompleted,
      ~onViewClick,
      _children,
    ) => {
  ...component,
  render: _self => {
    let activeTodoWord = count == 1 ? "item" : "items";
    let clearButton =
      completedCount > 0
        ? <button
            className="clear-completed" onClick={_e => onClearCompleted()}>
            {ReasonReact.string("Clear completed")}
          </button>
        : ReasonReact.null;
    let (all, active, completed) =
      switch ((nowShowing: TodoMvc_Model.view)) {
      | AllTodos => ("selected", "", "")
      | ActiveTodos => ("", "selected", "")
      | CompletedTodos => ("", "", "selected")
      };
    <footer className="footer">
      <span className="todo-count">
        <strong> {ReasonReact.string(string_of_int(count))} </strong>
        {ReasonReact.string(" " ++ activeTodoWord ++ " left")}
      </span>
      <ul className="filters">
        <li>
          <a
            onClick={e => {
              onViewClick(TodoMvc_Model.AllTodos);
              ReactEvent.Mouse.preventDefault(e);
            }}
            className=all>
            {ReasonReact.string("All")}
          </a>
        </li>
        {ReasonReact.string(" ")}
        <li>
          <a
            onClick={e => {
              onViewClick(TodoMvc_Model.ActiveTodos);
              ReactEvent.Mouse.preventDefault(e);
            }}
            className=active>
            {ReasonReact.string("Active")}
          </a>
        </li>
        {ReasonReact.string(" ")}
        <li>
          <a
            onClick={e => {
              onViewClick(TodoMvc_Model.CompletedTodos);
              ReactEvent.Mouse.preventDefault(e);
            }}
            className=completed>
            {ReasonReact.string("Completed")}
          </a>
        </li>
      </ul>
      clearButton
    </footer>;
  },
};
