Welcome to VerifiedReact! This is work in progress! Stay in touch via [VerifiedByAI](https://www.twitter.com/verifiedbyai)

![Verified React logo](/verified-react-logo.png)

# Intro

- Stage 1  (Counter and TodoMVC)
  - Reducing, with a couple of goals with reachability analysis
  - Hook state machine up to *actual* react view 

- Stage 2 - time travel
  - Fancy stuff with decomposition based on the states, and 'public' time travelling React libs

- Stage 3 - state space decomposition with jest testing
  - Coverage report of state space as hit by your jest tests.

# Setup

Make sure you have the latest version of `imandra-repl` installed via the [Imandra Installer](https://docs.imandra.ai/imandra-docs/notebooks/installation/), then run

    imandra-repl
    
and make sure it's starts up successfully (i.e. all updates are installed). Once it's started, quit it again with Ctrl-D. Next:

    npm install
    
to install the bucklescript compiler, `imandra-prelude` and Imandra client bindings for bucklescript. Then:
    
    npm run watch
    
You may see a few errors the first time you run watch - this I believe is due to an issue with `components` in `bsb.exe` as they are a WIP. However, if you run `npm run watch` multiple times, you should stop seeing errors after a few builds, and from then on incremental compilation will work sucessfully.

## The compilation chain

For runtime:

- `.iml` and `.ire` files are compiled to `.ml` files by `bsb` components, using `imandra-extract`  (which is bundled with `imandra-repl`).
- `.ml` and `.re` files are compiled to `.bs.js` via `bsb` itself.

## Verification tests:

- Tests are run via Jest on node.js using the compiled runtime JS files.
- As part of the test run, the Imandra client bucklescript bindings are used to spin up `imandra-http-server` (bundled with `imandra-repl`), which is an OCaml binary talking to Imandra's reasoning engine in the cloud.
- The HTTP Imandra client API is used to load 'raw' (uncompiled) `.iml` and `.ire` files into the running OCaml Imandra client, and perform verification statements.
- The verification results are captured and reported back as part of the Jest test run.

To run the verification goals:

    npm run test
