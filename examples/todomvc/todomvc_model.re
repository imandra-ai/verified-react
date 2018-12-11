type msg =
  | Increment
  | Decrement;

type model = {count: int};

let update = (model, msg) =>
  switch (msg) {
  | Increment => {count: model.count + 1}
  | Decrement => {count: model.count - 1}
  };

let goal_increment = model => {
  let new_model = update(model, Increment);
  new_model.count == model.count + 1;
};

let goal_decrement = model => {
  let new_model = update(model, Decrement);
  new_model.count == model.count - 1;
};
