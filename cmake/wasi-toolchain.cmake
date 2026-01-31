# WASI Toolchain file for Protocol Buffers
# Requires WASI SDK 29.0 or later

# Set system name for WASI
set(CMAKE_SYSTEM_NAME WASI)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR wasm32)

# WASI SDK path - can be overridden via -DWASI_SDK_PREFIX
if(NOT DEFINED WASI_SDK_PREFIX)
  if(DEFINED ENV{WASI_SDK_PREFIX})
    set(WASI_SDK_PREFIX "$ENV{WASI_SDK_PREFIX}")
  else()
    set(WASI_SDK_PREFIX "$ENV{HOME}/repos/wasi-sdk-bin-29.0")
  endif()
endif()

if(NOT EXISTS "${WASI_SDK_PREFIX}")
  message(FATAL_ERROR "WASI SDK not found at ${WASI_SDK_PREFIX}. Set WASI_SDK_PREFIX.")
endif()

# Set compilers
set(CMAKE_C_COMPILER "${WASI_SDK_PREFIX}/bin/clang")
set(CMAKE_CXX_COMPILER "${WASI_SDK_PREFIX}/bin/clang++")
set(CMAKE_AR "${WASI_SDK_PREFIX}/bin/llvm-ar")
set(CMAKE_RANLIB "${WASI_SDK_PREFIX}/bin/llvm-ranlib")
set(CMAKE_C_COMPILER_TARGET "wasm32-wasi")
set(CMAKE_CXX_COMPILER_TARGET "wasm32-wasi")

# Sysroot
set(CMAKE_SYSROOT "${WASI_SDK_PREFIX}/share/wasi-sysroot")

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
# Enable signal emulation for WASI (required by abseil debugging code)
set(CMAKE_C_FLAGS_INIT "-fno-exceptions -D_WASI_EMULATED_SIGNAL")
set(CMAKE_CXX_FLAGS_INIT "-fno-exceptions -fno-rtti -D_WASI_EMULATED_SIGNAL")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-lwasi-emulated-signal")

# Define to disable plugin support (subprocess fork/exec not available in WASI)
add_compile_definitions(PROTOBUF_DISABLE_PLUGINS=1)

# WASI doesn't have threads
set(CMAKE_THREAD_LIBS_INIT "")
set(CMAKE_HAVE_THREADS_LIBRARY 0)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 0)
set(THREADS_PREFER_PTHREAD_FLAG OFF)

# Disable features not available in WASI
set(HAVE_PTHREAD 0)

# Find programs on the host
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
