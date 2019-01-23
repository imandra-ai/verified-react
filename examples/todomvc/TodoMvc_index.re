module D = TodoMvcModel.Decode(Decoders_bs.Decode);

ReactDOMRe.renderToElementWithId(
  <InstanceBrowser
    body=(
      instanceJson => {
        let instance =
          switch (instanceJson) {
          | None => None
          | Some(j) =>
            Decoders_bs.Decode.decode_value(D.state, j)
            |> Belt.Result.getExn
            |> (x => Some(x))
          };
        <section className="todoapp">
          <App customInitialState=instance />
        </section>;
      }
    )
    serverInfo=Imandra_client.Server_info.{
                 port: 3000,
                 base_url: "http://localhost:3000",
               }
    examples=[]
    instancePrinterFn="state_to_json"
    instanceType="state"
    setupScriptPath="examples/todomvc/TodoMvcModel_setup.ire"
  />,
  "main",
);
