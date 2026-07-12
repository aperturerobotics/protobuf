// Copyright 2026 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// WASI reactors are single-threaded. Abseil omits these synchronization
// definitions when mmap-backed LowLevelAlloc is unavailable, but protobuf still
// references them through absl::Mutex. The uncontended reactor path only needs
// one correctly aligned identity and nonblocking semaphore operations.

#include "absl/base/config.h"
#include "absl/base/internal/thread_identity.h"
#include "absl/synchronization/internal/create_thread_identity.h"
#include "absl/synchronization/internal/kernel_timeout.h"
#include "absl/synchronization/internal/per_thread_sem.h"

extern "C" {

void ABSL_INTERNAL_C_SYMBOL(AbslInternalPerThreadSemInit)(
    absl::base_internal::ThreadIdentity* /*identity*/) {}

void ABSL_INTERNAL_C_SYMBOL(AbslInternalPerThreadSemPost)(
    absl::base_internal::ThreadIdentity* /*identity*/) {}

bool ABSL_INTERNAL_C_SYMBOL(AbslInternalPerThreadSemWait)(
    absl::synchronization_internal::KernelTimeout /*timeout*/) {
  return true;
}

void ABSL_INTERNAL_C_SYMBOL(AbslInternalPerThreadSemPoke)(
    absl::base_internal::ThreadIdentity* /*identity*/) {}

}  // extern "C"

namespace absl {
ABSL_NAMESPACE_BEGIN
namespace synchronization_internal {

base_internal::ThreadIdentity* CreateThreadIdentity() {
  alignas(base_internal::PerThreadSynch::kAlignment)
      static base_internal::ThreadIdentity identity = {};
  return &identity;
}

}  // namespace synchronization_internal
ABSL_NAMESPACE_END
}  // namespace absl
