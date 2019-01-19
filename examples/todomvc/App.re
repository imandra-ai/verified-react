type state = {
  it: TodoMvcModel.state,
  customInitialState: option(TodoMvcModel.state),
};

type action =
  | NewTodoKeyDown(int)
  | Model(TodoMvcModel.msg);

/* Component template declaration.
   Needs to be **after** state and action declarations! */
let component = ReasonReact.reducerComponent("Example");

let defaultInitialState = TodoMvcModel.initialState;

/* greeting and children are props. `children` isn't used, therefore ignored.
   We ignore it by prepending it with an underscore */
let make = (~customInitialState, _children) => {
  ...component,
  initialState: () =>
    switch (customInitialState) {
    | None => {it: defaultInitialState, customInitialState: None}
    | Some(s) => {it: s, customInitialState: Some(s)}
    },
  willReceiveProps: self =>
    switch (customInitialState, self.state.customInitialState) {
    | (Some(s1) as a, b) when a != b => {
        it: s1,
        customInitialState: Some(s1),
      }
    | _ => self.state
    },
  reducer: (action, state) =>
    switch (action) {
    | NewTodoKeyDown(keyCode) =>
      ReasonReact.SideEffects(
        (
          self =>
            if (keyCode == 13 /* enter key*/) {
              self.send(Model(AddNewTodo));
            }
        ),
      )
    | Model(m) =>
      let newState = TodoMvcModel.update(m, state.it);
      ReasonReact.Update({...state, it: newState});
    },
  render: self => {
    let activeTodoCount = Z.to_int(TodoMvcModel.activeCount(self.state.it));
    let completedTodoCount =
      Z.to_int(TodoMvcModel.completedCount(self.state.it));
    let shownTodos = TodoMvcModel.shownTodos(self.state.it);
    <div>
      <header className="header">
        <h1> {ReasonReact.string("todos")} </h1>
        <input
          className="new-todo"
          placeholder="What needs to be done?"
          value={self.state.it.newTodoText}
          onKeyDown={
            event =>
              self.send(NewTodoKeyDown(ReactEvent.Keyboard.which(event)))
          }
          onChange={
            event =>
              self.send(
                Model(EditNewTodo(ReactEvent.Form.target(event)##value)),
              )
          }
          autoFocus=true
        />
      </header>
      {
        if (Z.to_int(TodoMvcModel.totalCount(self.state.it)) > 0) {
          <section className="main">
            <input
              id="toggle-all"
              className="toggle-all"
              type_="checkbox"
              checked={activeTodoCount == 0}
              onChange={_e => self.send(Model(ToggleAll))}
            />
            <label htmlFor="toggle-all" />
            <ul className="todo-list">
              {
                Belt.List.map(shownTodos, todo =>
                  <TodoItem
                    key={Z.to_string(todo.id)}
                    todo
                    onToggle={() => self.send(Model(ToggleTodo(todo.id)))}
                    onDestroy={() => self.send(Model(DeleteTodo(todo.id)))}
                    onStartEdit={
                      () => self.send(Model(StartEditingTodo(todo.id)))
                    }
                    onEdit={s => self.send(Model(EditTodo(todo.id, s)))}
                    editing={todo.editing}
                    onSave={
                      () => self.send(Model(DoneEditingTodo(todo.id)))
                    }
                  />
                )
                |> Array.of_list
                |> ReasonReact.array
              }
            </ul>
          </section>;
        } else {
          ReasonReact.null;
        }
      }
      {
        if (activeTodoCount > 0 || completedTodoCount > 0) {
          <TodoFooter
            count=activeTodoCount
            completedCount=completedTodoCount
            nowShowing={self.state.it.viewing}
            onClearCompleted={_e => self.send(Model(ClearCompleted))}
            onViewClick={v => self.send(Model(View(v)))}
          />;
        } else {
          ReasonReact.null;
        }
      }
    </div>;
  },
};
