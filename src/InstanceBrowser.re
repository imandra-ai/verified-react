open Css;

module I = Imandra_client;

type action =
  | Noop
  | InitialiseError(I.Error.t)
  | Initialised
  | QueryInstance(string)
  | InstanceReturned(I.Api.Response.instance_result)
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

let serverInfo: I.Server_info.t = {
  port: 3000,
  base_url: "http://localhost:3000",
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
      I.Eval.by_src(
        ~syntax=Imandra_client.Api.Reason,
        ~src=Printf.sprintf("#use \"%s\"", setupScriptPath),
        serverInfo,
      )
      |> Js.Promise.then_(v => {
           switch (v) {
           | Belt.Result.Ok(_) => self.send(Initialised)
           | Belt.Result.Error(e) => self.send(InitialiseError(e))
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
      Js.Console.error(I.Error.pp_str(e));
      ReasonReact.Update({...s, init: Error(I.Error.pp_str(e))});
    | QueryInstance(queryStr) =>
      ReasonReact.UpdateWithSideEffects(
        {...s, instanceFetch: Loading, query: queryStr},
        self => {
          let _p =
            I.Instance.by_src(
              ~syntax=Imandra_client.Api.Reason,
              ~src=Printf.sprintf("(x : %s) => %s", instanceType, queryStr),
              ~instance_printer={name: instancePrinterFn, cx_var_name: "x"},
              serverInfo,
            )
            |> Js.Promise.then_(v => {
                 switch (v) {
                 | Belt.Result.Ok(r) => self.send(InstanceReturned(r))
                 | Belt.Result.Error(
                     I.Error.Imandra_error({
                       error:
                         "Imandra_reason_parser__Reason_syntax_util.Error(_, _)",
                     }),
                   ) =>
                   self.send(InstanceFetchError("Reason parse error"))
                 | Belt.Result.Error(e) =>
                   self.send(InstanceFetchError(I.Error.pp_str(e)))
                 };
                 Js.Promise.resolve();
               });
          ();
        },
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
      | I.Api.Response.I_sat(i) =>
        ReasonReact.Update({
          ...s,
          instanceFetch: Loaded,
          instanceFeedback: None,
          instance: Json.parse(i.instance.printed |> Belt.Option.getExn),
        })
      | I.Api.Response.I_unknown(u) =>
        ReasonReact.Update({
          ...s,
          instanceFetch: Loaded,
          instanceFeedback:
            Some(Printf.sprintf("Instance unknown: %s", u.unknown_reason)),
        })
      | I.Api.Response.I_unsat =>
        ReasonReact.Update({
          ...s,
          instanceFetch: Loaded,
          instanceFeedback: Some("Unsat."),
        })
      }
    },

  render: self => {
    let example = s =>
      <button className="app__button--example "
        onClick={_e => self.send(QueryInstance(s))}
        disabled={
          switch (self.state.init) {
          | Loaded => false
          | _ => true
          }
        }>
        <pre> {ReasonReact.string(s)} </pre>
      </button>;
    
    
    
      <div className="app__columns">
        <div className="app__column"> {body(self.state.instance)} </div>
        <div className="app__column">
        
        <h1 className="app__title"> {ReasonReact.string("Instance Browser")} </h1>
        
        <h2 className="app__description"> {ReasonReact.string("Make sure ")}
          <a
            href="https://docs.imandra.ai/imandra-docs/notebooks/installation-simple/"
            className={"app__description--link " ++ {style([fontFamily("monospace"), whiteSpace(`pre)])}}>
            {ReasonReact.string("imandra-http-server -reason")}
          </a>
          {ReasonReact.string(
             " is running in the background on localhost:3000",
           )}
        </h2>

        <h2 className="app__call-to-action"> {ReasonReact.string("Enter a ReasonML instance query:")} </h2>
        
        <div className={"app__query-header " ++ { style([
            display(flexBox),
            justifyContent(spaceBetween),
            marginBottom(px(8)),
          ])}}>
          <pre>
            {ReasonReact.string(Printf.sprintf("x : %s  =>", instanceType))}
          </pre>
          <div className={style([display(flexBox), alignItems(center)])}>
            <div className={style([fontSize(px(8)), marginRight(px(5))])}>
              {ReasonReact.string("imandra-http-server connection")}
            </div>
            <div className={style([
                borderRadius(pct(50.)),
                height(px(16)),
                width(px(16)),
                backgroundColor(
                  switch (self.state.init) {
                  | Loading => hex("f33333")
                  | Loaded => hex("22e0b6")
                  | Error(_) => hex("f33333")
                  },
                ),
              ])}
            />
          </div>
        </div>
        <div className={style([display(flexBox), flexDirection(column)])}>
          <textarea
            disabled={
              switch (self.state.init) {
              | Loaded => false
              | _ => true
              }
            }
            className={ "app__query " ++ {style([height(px(100)), padding(px(5))])}}
            value={self.state.query}
            onChange={event =>
              self.send(QueryInstance(ReactEvent.Form.target(event)##value))
            }
          />
        </div>
        <div
          className={style([
            display(flexBox),
            width(pct(100.)),
            justifyContent(spaceBetween),
          ])}>
          <div
            className={style([
              width(pct(70.)),
              fontSize(px(10)),
              paddingTop(px(5)),
              fontFamily("monospace"),
              whiteSpace(`pre),
            ])}>
            {ReasonReact.string(
               switch (self.state.instanceFeedback) {
               | None => ""
               | Some(f) => f
               },
             )}
          </div>
          <div
            className={style([
              display(flexBox),
              alignItems(center),
              marginTop(px(10)),
            ])}>
            <div className={style([fontSize(px(8)), marginRight(px(5))])}>
              {ReasonReact.string("Query result")}
            </div>
            <div
              className={style([
                borderRadius(pct(50.)),
                height(px(16)),
                width(px(16)),
                backgroundColor(
                  switch (self.state.instanceFetch) {
                  | Loading => hex("0088FF")
                  | Waiting => lightgrey
                  | Loaded => hex("22e0b6")
                  | Error => hex("f33333")
                  },
                ),
              ])}
            />
          </div>
        </div>

        <div className="app__examples">
          <h3 className="app__call-to-action"> {ReasonReact.string("or try these examples:")}</h3>

          {if (List.length(examples) > 0) {
             let children =
               List.map(e => <li> {example(e)} </li>, examples)
               |> Array.of_list;
             <ul> ...children </ul>;
           } else {
             <div />;
           }}

        </div>



      </div>
    </div>;
  },
};
