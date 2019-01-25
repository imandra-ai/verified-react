let clear_imandra_process : unit -> unit Js.Undefined.t = [%raw fun x -> "{delete global.imandraNodeProcess; return }"]

let default = fun () ->
  let si = Imandra_client.Server_info.from_file () |> Belt.Result.getExn in
  Imandra_client.shutdown si
  |> Js.Promise.then_ (fun _ ->
      Imandra_client.Server_info.cleanup ();
      clear_imandra_process () |> ignore;
      Js.Promise.resolve ()
    )
  |> ignore
