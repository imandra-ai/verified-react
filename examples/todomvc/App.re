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
  render: self =>
    <div>
      <header className="header">
        <h1> (ReasonReact.string("todos")) </h1>
        <input
          className="new-todo"
          placeholder="What needs to be done?"
          value=self.state.it.newTodoText
          onKeyDown=(
            event =>
              self.send(NewTodoKeyDown(ReactEvent.Keyboard.which(event)))
          )
          onChange=(
            event =>
              self.send(
                Model(EditNewTodo(ReactEvent.Form.target(event)##value)),
              )
          )
          autoFocus=true
        />
      </header>
      (
        if (List.length(self.state.it.todos) > 0) {
          <section className="main">
            <input id="toggle-all" className="toggle-all" type_="checkbox" />
            <label htmlFor="toggle-all" />
            <ul className="todo-list" />
          </section>;
        } else {
          ReasonReact.null;
        }
      )
      {
        let activeTodoCount = TodoMvcModel.activeCount(self.state.it);
        let completedTodoCount = TodoMvcModel.completedCount(self.state.it);
        if (Z.to_int(activeTodoCount) > 0 || Z.to_int(completedTodoCount) > 0) {
          <TodoFooter
            count=activeTodoCount
            completedCount=completedTodoCount
            nowShowing=self.state.it.viewing
            onClearCompleted=(_e => self.send(Model(ClearCompleted)))
            onViewClick=(v => self.send(Model(View(v))))
          />;
        } else {
          ReasonReact.null;
        };
      }
    </div>,
};
