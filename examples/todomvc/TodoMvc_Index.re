module D = TodoMvc_Model.Decode(Decoders_bs.Decode);

ReactDOMRe.renderToElementWithId(
  <InstanceBrowser
    body={instanceJson => {
      let instance =
        switch (instanceJson) {
        | None => None
        | Some(j) =>
          Decoders_bs.Decode.decode_value(D.state, j)
          |> Belt.Result.getExn
          |> (x => Some(x))
        };
      <div className="instance">
        <section className="todoapp">
          <TodoMvc_App customInitialState=instance />
        </section>
        <div className="instance__description">
          <h2 className="instance__subtitle">
            {ReasonReact.string(
               "Generate instances of TodoMVC state based on ",
             )}
            <a
              className="instance__subtitle--link"
              href="https://github.com/AestheticIntegration/verified-react/blob/master/examples/todomvc/TodoMvc_Model.ire">
              {ReasonReact.string("the source code of the app logic")}
            </a>
            {ReasonReact.string(
               " which has been loaded into Imandra. A few helper functions are defined in ",
             )}
            <a
              className="instance__subtitle--link"
              href="https://github.com/AestheticIntegration/verified-react/blob/master/examples/todomvc/TodoMvc_Setup.ire">
              {ReasonReact.string("the instance setup file")}
            </a>
            {ReasonReact.string(" to help with writing logical constraints.")}
          </h2>
        </div>
      </div>;
    }}
    serverInfo=Imandra_client.Server_info.{
      port: 3000,
      base_url: "http://localhost:3000",
    }
    examples=[
      "num_todos(x, 5)",
      "with_todo_text(x)",
      "num_todos(x, 5) && with_todo_text(x)",
      "num_todos(x, 5) && with_todo_text(x) && empty_new_todo(x)",
      "num_todos(x, 5) && with_todo_text(x) && completedCount(x) == 3 && x.viewing == AllTodos",
      {|switch (update(EditNewTodo("Add a new todo"), initialState)) { | Ok(x0) => update(AddNewTodo, x0) == Ok(x) | _ => false } |},
    ]
    instancePrinterFn="state_to_json"
    instanceType="state"
    setupScriptPath="examples/todomvc/TodoMvc_Setup.ire"
  />,
  "main",
);