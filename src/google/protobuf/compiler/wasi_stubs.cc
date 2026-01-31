// Stub implementations for Abseil threading symbols not available in WASI.
// WASI is single-threaded, so these are no-ops or minimal implementations.

#include <cstddef>
#include <cstdlib>

extern "C" {

// Per-thread semaphore stubs (single-threaded, so these are no-ops)
void AbslInternalPerThreadSemWait(void* /*waiter*/, void* /*how_long*/) {
  // No-op: single-threaded environment
}

void AbslInternalPerThreadSemPost(void* /*waiter*/) {
  // No-op: single-threaded environment
}

}  // extern "C"

namespace absl {
namespace synchronization_internal {

// Thread identity stub - in single-threaded WASI, we use a static identity
struct ThreadIdentity {
  void* per_thread_synch;
  void* waited;
  int ticker;
  int wait_start;
  bool is_idle;
};

static ThreadIdentity g_single_thread_identity = {};

void* CreateThreadIdentity() {
  return &g_single_thread_identity;
}

}  // namespace synchronization_internal

namespace base_internal {

// LowLevelAlloc stubs - use standard malloc/free
// In WASI, we don't need the arena-based allocation since there's no signal handling

struct LowLevelAllocArena {};

void LowLevelAllocFree(void* p) {
  std::free(p);
}

void* LowLevelAllocSigSafeArena() {
  return nullptr;  // No signal-safe arena needed in WASI
}

void LowLevelAllocInitSigSafeArena() {
  // No-op
}

void* LowLevelAllocAllocWithArena(size_t size, void* /*arena*/) {
  return std::malloc(size);
}

}  // namespace base_internal
}  // namespace absl

// C++ mangled name stubs that match what the linker expects
// These are the actual symbols that are undefined

namespace absl {
namespace base_internal {

class LowLevelAlloc {
 public:
  struct Arena;
  static void Free(void* p);
  static void* AllocWithArena(std::size_t request, Arena* arena);
};

void LowLevelAlloc::Free(void* p) {
  std::free(p);
}

void* LowLevelAlloc::AllocWithArena(std::size_t request, Arena* /*arena*/) {
  return std::malloc(request);
}

void InitSigSafeArena() {
  // No-op
}

void* SigSafeArena() {
  return nullptr;
}

}  // namespace base_internal
}  // namespace absl
