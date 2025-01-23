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