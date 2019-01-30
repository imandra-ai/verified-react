open Css;
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
      <div>
        <section className="todoapp">
          <TodoMvc_App customInitialState=instance />
        </section>
        <div>
          {ReasonReact.string(
             "Generate instances of TodoMVC state based purely on ",
           )}
          <a
            href="https://github.com/AestheticIntegration/verified-react/blob/master/examples/todomvc/TodoMvc_Model.ire">
            {ReasonReact.string("the source code of the app logic")}
          </a>
          {ReasonReact.string(" which has been loaded into Imandra.")}
        </div>
      </div>;
    }}
    serverInfo=Imandra_client.Server_info.{
      port: 3000,
      base_url: "http://localhost:3000",
    }
    examples=[]
    instancePrinterFn="state_to_json"
    instanceType="state"
    setupScriptPath="examples/todomvc/TodoMvc_Setup.ire"
  />,
  "main",
);
