# VerifiedReact

Welcome to VerifiedReact! This is work in progress - stay in touch via [@VerifiedByAI](https://www.twitter.com/verifiedbyai).

![Verified React logo](/verified-react-logo.png)

## Ideas and aims

- [x] Stage 1  (Counter, TicTacToe)
  - [x] Automation of verification goals via `jest`. See:
    - `examples/simple/__tests__/SimpleModel_Goals.ml`
    - `examples/tictactoe/__tests__/TicTacToeLogic_Goals.re`
  - [x] Hook verified state machine up to React reducer component. See:
    - `examples/tictactoe/TicTacToe.re`
  
- [ ] Stage 2 
  - [ ] TodoMVC as a larger example
  - [ ] Displaying instances
  - [ ] Decomposition visualisation

- [ ] Stage 3
  - [ ] Collecting React reducer events from React unit test runs
  - [ ] Map reducer events back to state machine events, and visualise coverage on the decomposition
  - [ ] Coverage report of state space as hit by your jest tests

## Setup

Make sure you have the latest version of `imandra-repl` installed via the [Imandra Installer](https://docs.imandra.ai/imandra-docs/notebooks/installation/), then run

    imandra-repl
    
and make sure it's starts up successfully (i.e. all updates are installed). Once it's started, quit it again with Ctrl-D. Next:

    npm install
    
to install the bucklescript compiler, `imandra-prelude` and Imandra client bindings for bucklescript. Then:
    
    npm run watch
    
You may see a few errors the first time you run watch - this I believe is due to an issue with `components` in `bsb.exe` as they are a WIP. However, if you run `npm run watch` multiple times, you should stop seeing errors after a few builds, and from then on incremental compilation will work sucessfully.

## The compilation chain

For runtime:

- `.iml` and `.ire` files are compiled to `.ml` files using `imandra-extract` (which is bundled with the [Imandra Installer](https://docs.imandra.ai/imandra-docs/notebooks/installation/)), and integrated into the build via `bsb` components.
- `.ml` and `.re` files are compiled to `.bs.js` via `bsb` itself.
- `Imandra_prelude` is available automatically inside `imandra-http-server`
- When compiling to javascript, `Imandra_prelude` is provided by [imandra-prelude](https://github.com/AestheticIntegration/imandra-prelude), added as an npm/bucklescript dependency.

## Verification tests:

- Tests are run via Jest on node.js using the compiled runtime JS files.
- As part of the test run, the Imandra client bucklescript bindings (`bs-imandra-client`) are used to spin up `imandra-http-server` (which is bundled as part of the [Imandra Installer](https://docs.imandra.ai/imandra-docs/notebooks/installation/)), which is an OCaml binary talking to Imandra's reasoning engine in the cloud.
- The HTTP Imandra client API is used to load `.iml` and `.ire` files into the running `imandra-http-server` OCaml process, and perform verification statements.
- The verification results are captured and reported back as part of the Jest test run.

To run the verification goals:

    npm run test
