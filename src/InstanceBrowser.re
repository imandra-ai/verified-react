open Css;

type action =
  | Noop
  | InitialiseError(string)
  | Initialised
  | QueryInstance(string)
  | InstanceReturned(Imandra_client.Instance.instanceResult)
  | InstanceFetchError(string);

type initState =
  | Loading
  | Error(string)
  | Loaded;

type fetchState =
  | Waiting
  | Loading
  | Error
  | Loaded;

type state = {
  init: initState,
  instanceFetch: fetchState,
  instance: option(Js.Json.t),
  instanceFeedback: option(string),
  query: string,
};

let component = ReasonReact.reducerComponent("InstanceBrowser");

let serverInfo: Imandra_client.ServerInfo.t = {
  port: 3000,
  baseUrl: "http://localhost:3000",
};

let make =
    (
      ~serverInfo,
      ~setupScriptPath,
      ~instanceType,
      ~instancePrinterFn,
      ~body,
      ~examples=[],
      _children,
    ) => {
  ...component,
  initialState: () => {
    init: Loading,
    instance: None,
    instanceFetch: Waiting,
    instanceFeedback: None,
    query: "",
  },
  didMount: self => {
    let _p =
      Imandra_client.Eval.bySrc(
        ~syntax=Imandra_client.Syntax.Reason,
        ~src=Printf.sprintf("#use \"%s\"", setupScriptPath),
        serverInfo,
      )
      |> Js.Promise.then_(v => {
           switch (v) {
           | Belt.Result.Ok(_) => self.send(Initialised)
           | Belt.Result.Error((e, _)) => self.send(InitialiseError(e))
           };
           Js.Promise.resolve();
         });
    ();
  },
  reducer: (action, s: state) =>
    switch (action) {
    | Noop => ReasonReact.Update(s)
    | Initialised => ReasonReact.Update({...s, init: Loaded})
    | InitialiseError(e) =>
      Js.Console.error(e);
      ReasonReact.Update({...s, init: Error(e)});
    | QueryInstance(queryStr) =>
      ReasonReact.UpdateWithSideEffects(
        {...s, instanceFetch: Loading, query: queryStr},
        (
          self => {
            let _p =
              Imandra_client.Instance.bySrc(
                ~syntax=Imandra_client.Syntax.Reason,
                ~src=Printf.sprintf("(x : %s) => %s", instanceType, queryStr),
                ~instancePrinter={name: instancePrinterFn, cx_var_name: "x"},
                serverInfo,
              )
              |> Js.Promise.then_(v => {
                   switch (v) {
                   | Belt.Result.Ok((r, _)) =>
                     self.send(InstanceReturned(r))
                   | Belt.Result.Error((
                       "Imandra_reason_parser__Reason_syntax_util.Error(_, _)",
                       _,
                     )) =>
                     self.send(InstanceFetchError("Reason parse error"))
                   | Belt.Result.Error((e, _)) =>
                     self.send(InstanceFetchError(e))
                   };
                   Js.Promise.resolve();
                 });
            ();
          }
        ),
      )
    | InstanceFetchError(e) =>
      Js.Console.error(e);
      ReasonReact.Update({
        ...s,
        instanceFetch: Error,
        instanceFeedback: Some(Printf.sprintf("Error: %s", e)),
      });
    | InstanceReturned(r) =>
      switch (r) {
      | Imandra_client.Instance.Sat(i) =>
        ReasonReact.Update({
          ...s,
          instanceFetch: Loaded,
          instanceFeedback: None,
          instance: Json.parse(i.instance.printed |> Belt.Option.getExn),
        })
      | Imandra_client.Instance.Unknown(u) =>
        ReasonReact.Update({
          ...s,
          instanceFetch: Loaded,
          instanceFeedback:
            Some(Printf.sprintf("Instance unknown: %s", u.reason)),
        })
      | Imandra_client.Instance.Unsat =>
        ReasonReact.Update({
          ...s,
          instanceFetch: Loaded,
          instanceFeedback: Some("Unsat."),
        })
      }
    },
  render: self => {
    let example = s =>
      <button
        onClick=(_e => self.send(QueryInstance(s)))
        disabled=(
          switch (self.state.init) {
          | Loaded => false
          | _ => true
          }
        )>
        <pre> (ReasonReact.string(s)) </pre>
      </button>;
    <div
      className=(
        style([display(flexBox), flexDirection(column), fontSize(px(14))])
      )>
      <div> (body(self.state.instance)) </div>
      <div
        className=(
          style([
            display(flexBox),
            flexDirection(column),
            marginTop(px(20)),
          ])
        )>
        <div
          className=(
            style([
              fontSize(px(16)),
              maxWidth(px(400)),
              marginBottom(px(20)),
            ])
          )>
          (ReasonReact.string("Generate instances based purely on "))
          <a
            href="https://github.com/AestheticIntegration/verified-react/blob/master/examples/tictactoe/TicTacToeLogic.ire">
            (ReasonReact.string("the source code of the game logic"))
          </a>
          (ReasonReact.string(" which has been loaded into Imandra."))
        </div>
        <div>
          (ReasonReact.string("Make sure "))
          <a
            href="https://docs.imandra.ai/imandra-docs/notebooks/installation-simple/"
            className=(style([fontFamily("monospace"), whiteSpace(`pre)]))>
            (ReasonReact.string("imandra-http-server"))
          </a>
          (
            ReasonReact.string(
              " is running in the background on localhost:3000",
            )
          )
        </div>
        <div> (ReasonReact.string("Enter a ReasonML instance query:")) </div>
        <div
          className=(
            style([display(flexBox), justifyContent(spaceBetween)])
          )>
          <pre> (ReasonReact.string("x : game_state  =>")) </pre>
          <div className=(style([display(flexBox), alignItems(center)]))>
            <div className=(style([fontSize(px(8)), marginRight(px(5))]))>
              (ReasonReact.string("imandra-http-server connection"))
            </div>
            <div
              className=(
                style([
                  borderRadius(pct(50.)),
                  height(px(20)),
                  width(px(20)),
                  backgroundColor(
                    switch (self.state.init) {
                    | Loading => red
                    | Loaded => green
                    | Error(_) => red
                    },
                  ),
                ])
              )
            />
          </div>
        </div>
        <div className=(style([display(flexBox), flexDirection(column)]))>
          <textarea
            disabled=(
              switch (self.state.init) {
              | Loaded => false
              | _ => true
              }
            )
            className=(style([height(px(100)), padding(px(5))]))
            value=self.state.query
            onChange=(
              event =>
                self.send(
                  QueryInstance(ReactEvent.Form.target(event)##value),
                )
            )
          />
        </div>
        <div
          className=(
            style([
              display(flexBox),
              width(pct(100.)),
              justifyContent(spaceBetween),
            ])
          )>
          <div
            className=(
              style([
                width(pct(70.)),
                fontSize(px(10)),
                paddingTop(px(5)),
                fontFamily("monospace"),
                whiteSpace(`pre),
              ])
            )>
            (
              ReasonReact.string(
                switch (self.state.instanceFeedback) {
                | None => ""
                | Some(f) => f
                },
              )
            )
          </div>
          <div
            className=(
              style([
                display(flexBox),
                alignItems(center),
                marginTop(px(10)),
              ])
            )>
            <div className=(style([fontSize(px(8)), marginRight(px(5))]))>
              (ReasonReact.string("Query result"))
            </div>
            <div
              className=(
                style([
                  borderRadius(pct(50.)),
                  height(px(20)),
                  width(px(20)),
                  backgroundColor(
                    switch (self.state.instanceFetch) {
                    | Loading => blue
                    | Waiting => grey
                    | Loaded => green
                    | Error => red
                    },
                  ),
                ])
              )
            />
          </div>
        </div>
        <div className=(style([marginTop(px(20))]))>
          (ReasonReact.string("or try these examples:"))
        </div>
        <div>
          (
            if (List.length(examples) > 0) {
              let children =
                List.map(e => <li> (example(e)) </li>, examples)
                |> Array.of_list;
              <ul> ...children </ul>;
            } else {
              <div />;
            }
          )
        </div>
      </div>
    </div>;
  },
};
