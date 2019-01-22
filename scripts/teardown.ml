let get_imandra_process : unit -> Node.Child_process.spawnResult Js.Undefined.t = [%raw fun x -> "{return global.imandraNodeProcess}"]

let default = fun () ->
  let np = get_imandra_process () in
  Imandra_client.Server_info.cleanup ();
  match (Js.Undefined.toOption np) with
  | None ->
    Js.Promise.resolve()
  | Some np ->
    begin
      Imandra_client.stop np
      |> Js.Promise.then_ (fun _ -> Js.Promise.resolve ())
    end
