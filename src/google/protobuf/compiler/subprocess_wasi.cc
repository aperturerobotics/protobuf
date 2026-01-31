// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

// WASI implementation of subprocess functionality.
// Uses host-imported functions to spawn plugin processes.

#include "google/protobuf/compiler/subprocess.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>

#include "google/protobuf/message.h"

// Host-imported functions for plugin subprocess communication.
// These are provided by the Go/wazero host and allow WASI to spawn
// native processes for protoc plugins.
//
// The communicate function:
// - Takes program name and serialized input
// - Returns serialized output or error message
// - Returns 0 on success, non-zero on failure
extern "C" {

// Communicate with a plugin subprocess.
// Parameters:
//   program_ptr, program_len: Plugin program name (e.g., "protoc-gen-go")
//   search_path: 1 to search PATH, 0 for exact path
//   input_ptr, input_len: Serialized CodeGeneratorRequest
//   output_ptr: Pointer to receive output buffer (allocated by host, caller must free)
//   output_len: Pointer to receive output length
//   error_ptr: Pointer to receive error buffer (allocated by host, caller must free)
//   error_len: Pointer to receive error length
// Returns: 0 on success, plugin exit code on failure, -1 on spawn error
__attribute__((import_module("protoc"), import_name("plugin_communicate")))
int32_t protoc_plugin_communicate(
    const char* program_ptr, uint32_t program_len,
    int32_t search_path,
    const char* input_ptr, uint32_t input_len,
    char** output_ptr, uint32_t* output_len,
    char** error_ptr, uint32_t* error_len);

}  // extern "C"

namespace google {
namespace protobuf {
namespace compiler {

Subprocess::Subprocess()
    : search_mode_(SEARCH_PATH), child_pid_(-1), child_stdin_(-1), child_stdout_(-1) {}

Subprocess::~Subprocess() {}

void Subprocess::Start(const std::string& program, SearchMode search_mode) {
  program_ = program;
  search_mode_ = search_mode;
}

bool Subprocess::Communicate(const Message& input, Message* output,
                             std::string* error) {
  // Serialize the input message
  std::string input_data;
  if (!input.SerializeToString(&input_data)) {
    *error = "Failed to serialize request.";
    return false;
  }

  // Call the host function
  char* output_ptr = nullptr;
  uint32_t output_len = 0;
  char* error_ptr = nullptr;
  uint32_t error_len = 0;

  int32_t result = protoc_plugin_communicate(
      program_.data(), static_cast<uint32_t>(program_.size()),
      (search_mode_ == SEARCH_PATH) ? 1 : 0,
      input_data.data(), static_cast<uint32_t>(input_data.size()),
      &output_ptr, &output_len,
      &error_ptr, &error_len);

  if (result != 0) {
    if (error_ptr != nullptr && error_len > 0) {
      error->assign(error_ptr, error_len);
      std::free(error_ptr);
    } else {
      *error = "Plugin failed with exit code " + std::to_string(result);
    }
    if (output_ptr != nullptr) {
      std::free(output_ptr);
    }
    return false;
  }

  // Parse the output
  if (output_ptr == nullptr || output_len == 0) {
    *error = "Plugin returned empty output.";
    return false;
  }

  std::string output_data(output_ptr, output_len);
  std::free(output_ptr);
  if (error_ptr != nullptr) {
    std::free(error_ptr);
  }

  if (!output->ParseFromString(output_data)) {
    *error = "Plugin output is unparseable.";
    return false;
  }

  return true;
}

}  // namespace compiler
}  // namespace protobuf
}  // namespace google
