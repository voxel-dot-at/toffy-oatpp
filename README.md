# Toffy-Oatpp API Project

## Overview

The `toffy-oatpp` project is a C++ API built using Toffy and Oatpp, providing an interface to interact with real-time images and settings. It is designed to work with a web UI built using Vue 3, Vite, and Vuetify, which retrieves and displays data from this API. The API project exposes endpoints that allow for easy access to images and settings, which are presented in a clean and dynamic web interface.

## Project Structure

- **CMake** (build project)
- **Toffy** (used for API creation)
- **Oatpp** (used for HTTP handling)
- **Swagger** (for API documentation)

## Project Setup

Follow these steps to build the `toffy-oatpp` API project:

### Create a build directory and run CMake:

   ```sh
   mkdir build
   cd build
   cmake ..
   ```

### Build the project:

   ```sh
   cd ..
   make -j$(nproc) -C build
   ```

### Run the API server:

   ```sh
   ./build/app/test_oatpp
   ```

### The API will now be running and accessible. You can view the Swagger API documentation by navigating to:

   http://localhost:8000/swagger/ui#/

## Branches

- **`main`** — the line everything builds against and pins to: **`v0.1.0`**, the toffy 2.0 line.
  `find_package(toffy_oatpp 0.1 REQUIRED)` resolves against an install of it and pulls toffy in
  transitively, so consumers do not repeat the version gate. `VERSION` and `SOVERSION` are both
  the full version, as in toffy: a new release means rebuilding the chain, not relinking it.
- **`feature/toffy-upgrade`** — the toffy 2.0 upgrade: `find_package(toffy 2.0)`, `FilterBank::add(FilterPtr)`
  ownership, the `${toffy_DEFINITIONS}` Eigen flags, transitive `find_dependency` in the installed
  package, Boost `thread` dropped, four files that could not compile deleted. **Landed on `main`
  2026-10-30 by fast-forward** (`5e4b284..d32bcde`), and `v0.1.0` was cut from `main` afterwards —
  never from the branch, because a published tag is never moved. The branch is kept as the review
  history; it is not a place new work goes.
- **`next` — dropped 2026-10-28.** Fifteen commits of oatpp-socketio/websocket support, dynamic
  swagger URLs and a `WebApi` pointer refactor, branched from `main` in February 2025 and silent
  since `3a97d1b` ("count 1", 2025-04-25). It was never released, never consumed, and never
  carried any of the toffy 2.0 work; keeping it alive cost a decision every time anyone asked
  "what does `main` carry?" — which is the one question a version tag has to answer.

  Nothing was deleted to drop it. The work is `3a97d1b` and its fourteen parents, archived as the
  local tag `archive/next-2025-04-25` and in a `git bundle` beside the repository; the branch ref
  itself is no longer on `origin` (`git ls-remote` lists `main` and `feature/toffy-upgrade`
  only), which is safe precisely because the tag and the bundle exist. To revive it:
  branch from that tag and rebase onto current `main`, expecting three conflicts —
  `CMakeLists.txt` (mechanical), `src/CMakeLists.txt` (the deleted `webListener.cpp`), and
  `src/globals.cpp`, which is the interesting one: `state.webAdap` is a raw `WebAdapter*`, and
  since the `FilterPtr` change the **bank owns that object**, so the revived code needs to know
  who keeps it alive. The socket.io line also needs `oatpp-websocket` and `oatpp-socketio`
  installed wherever it is built.