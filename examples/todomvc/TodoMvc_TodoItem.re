let component = ReasonReact.statelessComponent("TodoItemRe");

let make =
    (
      ~todo: TodoMvc_Model.todo,
      ~editing,
      ~onDestroy,
      ~onSave,
      ~onStartEdit,
      ~onEdit,
      ~onToggle,
      _children,
    ) => {
  ...component,
  render: _self => {
    let className =
      [todo.done_ ? "completed" : "", editing ? "editing" : ""]
      |> String.concat(" ");
    <li className>
      <div className="view">
        <input
          className="toggle"
          type_="checkbox"
          checked={todo.done_}
          onChange={_e => onToggle()}
        />
        <label onDoubleClick={_event => onStartEdit()}>
          {ReasonReact.string(todo.text)}
        </label>
        <button className="destroy" onClick={_e => onDestroy()} />
      </div>
      <input
        className="edit"
        value={todo.text}
        onBlur={_event => onSave()}
        onChange={event => onEdit(ReactEvent.Form.target(event)##value)}
      />
    </li>;
  },
};
