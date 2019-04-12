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

let serverInfo: I.Server_info.t = {
  port: 3000,
  base_url: "http://localhost:3000",
};

let init = () => {
  init: Loading,
  instance: None,
  instanceFetch: Waiting,
  instanceFeedback: None,
  query: "",
};

let reducer = (s: state, action) =>
  switch (action) {
  | Noop => s
  | Initialised => {...s, init: Loaded}
  | InitialiseError(e) =>
    Js.Console.error(I.Error.pp_str(e));
    {...s, init: Error(I.Error.pp_str(e))};
  | QueryInstance(queryStr) => {
      ...s,
      instanceFetch: Loading,
      query: queryStr,
    }
  | InstanceFetchError(e) =>
    Js.Console.error(e);
    {
      ...s,
      instanceFetch: Error,
      instanceFeedback: Some(Printf.sprintf("Error: %s", e)),
    };
  | InstanceReturned(r) =>
    switch (r) {
    | I.Api.Response.I_sat(i) => {
        ...s,
        instanceFetch: Loaded,
        instanceFeedback: None,
        instance: Json.parse(i.instance.printed |> Belt.Option.getExn),
      }
    | I.Api.Response.I_unknown(u) => {
        ...s,
        instanceFetch: Loaded,
        instanceFeedback:
          Some(Printf.sprintf("Instance unknown: %s", u.unknown_reason)),
      }
    | I.Api.Response.I_unsat => {
        ...s,
        instanceFetch: Loaded,
        instanceFeedback: Some("Unsat."),
      }
    | I.Api.Response.I_unsat_upto(_) => {
        ...s,
        instanceFetch: Loaded,
        instanceFeedback: Some("Unsat."),
      }
    }
  };

[@react.component]
let make =
    (
      ~serverInfo,
      ~setupScriptPath,
      ~instanceType,
      ~instancePrinterFn,
      ~body,
      ~examples=[],
    )
    : React.element => {
  let (state, dispatch) = React.useReducer(reducer, init());
  let () =
    React.useEffect(()
      /* TODO: is this correct? */
      =>
        if (state.init === Loading) {
          Some(
            () => {
              let _p =
                I.Eval.by_src(
                  ~syntax=Imandra_client.Api.Reason,
                  ~src=Printf.sprintf("#use \"%s\"", setupScriptPath),
                  serverInfo,
                )
                |> Js.Promise.then_(v => {
                     switch (v) {
                     | Belt.Result.Ok(_) => dispatch(Initialised)
                     | Belt.Result.Error(e) => dispatch(InitialiseError(e))
                     };
                     Js.Promise.resolve();
                   });
              ();
            },
          );
        } else {
          None;
        }
      );

  /* Execute effect when state.query changes */
  let () =
    React.useEffect1(
      () =>
        Some(
          () => {
            let _p =
              I.Instance.by_src(
                ~syntax=Imandra_client.Api.Reason,
                ~src=
                  Printf.sprintf("(x : %s) => %s", instanceType, state.query),
                ~instance_printer={name: instancePrinterFn, cx_var_name: "x"},
                serverInfo,
              )
              |> Js.Promise.then_(v => {
                   switch (v) {
                   | Belt.Result.Ok(r) => dispatch(InstanceReturned(r))
                   | Belt.Result.Error(
                       I.Error.Imandra_error({
                         error:
                           "Imandra_reason_parser__Reason_syntax_util.Error(_, _)",
                       }),
                     ) =>
                     dispatch(InstanceFetchError("Reason parse error"))
                   | Belt.Result.Error(e) =>
                     dispatch(InstanceFetchError(I.Error.pp_str(e)))
                   };
                   Js.Promise.resolve();
                 });
            ();
          },
        ),
      [|state.query|],
    );

  let example = s =>
    <button
      className="app__button--example "
      onClick={_e => dispatch(QueryInstance(s))}
      disabled={
        switch (state.init) {
        | Loaded => false
        | _ => true
        }
      }>
      <pre> {React.string(s)} </pre>
    </button>;

  <div className="app__columns">
    <div className="app__column app__column--dark-bg ">
      {body(state.instance)}
    </div>
    <div className="app__column">
      <h1 className="app__title"> {React.string("Instance Browser")} </h1>
      <h2 className="app__description">
        {React.string("Make sure ")}
        <a
          href="https://docs.imandra.ai/imandra-docs/notebooks/installation-simple/"
          className={
            "app__description--link "
            ++ style([fontFamily("monospace"), whiteSpace(`pre)])
          }>
          {React.string("imandra-http-server -reason")}
        </a>
        {React.string(" is running in the background on localhost:3000")}
      </h2>
      <h2 className="app__call-to-action">
        {React.string("Enter a ReasonML instance query:")}
      </h2>
      <div
        className={
          "app__query-header "
          ++ style([
               display(flexBox),
               justifyContent(spaceBetween),
               marginBottom(px(8)),
             ])
        }>
        <pre>
          {React.string(Printf.sprintf("x : %s  =>", instanceType))}
        </pre>
        <div className={style([display(flexBox), alignItems(center)])}>
          <div className={style([fontSize(px(8)), marginRight(px(5))])}>
            {React.string("imandra-http-server connection")}
          </div>
          <div
            className={
              style([
                borderRadius(pct(50.)),
                height(px(16)),
                width(px(16)),
                backgroundColor(
                  switch (state.init) {
                  | Loading => hex("f33333")
                  | Loaded => hex("22e0b6")
                  | Error(_) => hex("f33333")
                  },
                ),
              ])
            }
          />
        </div>
      </div>
      <div className={style([display(flexBox), flexDirection(column)])}>
        <textarea
          disabled={
            switch (state.init) {
            | Loaded => false
            | _ => true
            }
          }
          className={
            "app__query "
            ++ style([
                 height(px(100)),
                 padding(px(5)),
                 fontSize(px(16)),
               ])
          }
          value={state.query}
          onChange={
            event =>
              dispatch(QueryInstance(ReactEvent.Form.target(event)##value))
          }
        />
      </div>
      <div
        className={
          style([
            display(flexBox),
            width(pct(100.)),
            justifyContent(spaceBetween),
          ])
        }>
        <div
          className={
            style([
              width(pct(70.)),
              maxWidth(pct(70.)),
              fontSize(px(10)),
              paddingTop(px(5)),
              fontFamily("monospace"),
              minHeight(px(30)),
            ])
          }>
          {
            React.string(
              switch (state.instanceFeedback) {
              | None => ""
              | Some(f) => f
              },
            )
          }
        </div>
        <div
          className={
            style([
              display(flexBox),
              alignItems(center),
              marginTop(px(10)),
            ])
          }>
          <div className={style([fontSize(px(8)), marginRight(px(5))])}>
            {React.string("Query result")}
          </div>
          <div
            className={
              style([
                borderRadius(pct(50.)),
                height(px(16)),
                width(px(16)),
                backgroundColor(
                  switch (state.instanceFetch) {
                  | Loading => hex("0088FF")
                  | Waiting => lightgrey
                  | Loaded => hex("22e0b6")
                  | Error => hex("f33333")
                  },
                ),
              ])
            }
          />
        </div>
      </div>
      <div className="app__examples">
        <h3 className="app__call-to-action">
          {React.string("or try these examples:")}
        </h3>
        {
          if (List.length(examples) > 0) {
            let children =
              List.map(e => <li> {example(e)} </li>, examples)
              |> Array.of_list
              |> React.array;
            <ul> children </ul>;
          } else {
            <div />;
          }
        }
      </div>
    </div>
  </div>;
};
