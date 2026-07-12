// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

// WASI reactor entry point for protoc.
// This file provides exported functions for running protoc as a WASI reactor,
// allowing multiple compilations per instance without reloading the module.

#include <cstdlib>

#include "absl/log/initialize.h"
#include "google/protobuf/compiler/command_line_interface.h"
#include "google/protobuf/compiler/cpp/generator.h"
#include "google/protobuf/compiler/csharp/csharp_generator.h"
#include "google/protobuf/compiler/python/generator.h"

// Must be included last.
#include "google/protobuf/port_def.inc"

namespace {

// Global CLI instance for the reactor
google::protobuf::compiler::CommandLineInterface* g_cli = nullptr;

// Built-in generators owned by the reactor.
google::protobuf::compiler::cpp::CppGenerator* g_cpp_generator = nullptr;
google::protobuf::compiler::csharp::Generator* g_csharp_generator = nullptr;
google::protobuf::compiler::python::Generator* g_python_generator = nullptr;

}  // namespace

extern "C" {

// Initialize the protoc reactor and register the built-in generators.
// Returns 0 on success, non-zero on error.
__attribute__((export_name("protoc_init")))
int protoc_init() {
  if (g_cli != nullptr) {
    // Already initialized
    return 0;
  }

  absl::InitializeLog();

  g_cli = new google::protobuf::compiler::CommandLineInterface();
  g_cli->AllowPlugins("protoc-");

  // C++ generator (built-in)
  g_cpp_generator = new google::protobuf::compiler::cpp::CppGenerator();
  g_cli->RegisterGenerator("--cpp_out", "--cpp_opt", g_cpp_generator,
                           "Generate C++ header and source.");

  g_csharp_generator = new google::protobuf::compiler::csharp::Generator();
  g_cli->RegisterGenerator("--csharp_out", "--csharp_opt", g_csharp_generator,
                           "Generate C# source file.");

  g_python_generator = new google::protobuf::compiler::python::Generator();
  g_cli->RegisterGenerator("--python_out", "--python_opt", g_python_generator,
                           "Generate Python source file.");

  return 0;
}

// Run protoc with the given arguments.
// protoc_init() must be called first.
// Returns the protoc exit code (0 on success).
__attribute__((export_name("protoc_run")))
int protoc_run(int argc, char* argv[]) {
  if (g_cli == nullptr) {
    return 1;  // Not initialized
  }
  return g_cli->Run(argc, argv);
}

// Destroy the protoc reactor and free resources.
__attribute__((export_name("protoc_destroy")))
void protoc_destroy() {
  delete g_cli;
  g_cli = nullptr;

  delete g_cpp_generator;
  g_cpp_generator = nullptr;

  delete g_csharp_generator;
  g_csharp_generator = nullptr;

  delete g_python_generator;
  g_python_generator = nullptr;
}

}  // extern "C"

#include "google/protobuf/port_undef.inc"
