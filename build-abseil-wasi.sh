#!/bin/bash
# Build Abseil for WASI
# This script must be run before build-wasi.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
WASI_SDK_PREFIX="${WASI_SDK_PREFIX:-$HOME/repos/wasi-sdk-bin-29.0}"
ABSEIL_SOURCE="${ABSEIL_SOURCE:-$SCRIPT_DIR/third_party/abseil-cpp}"
BUILD_DIR="${SCRIPT_DIR}/build-abseil-wasi"
INSTALL_DIR="${SCRIPT_DIR}/build-abseil-wasi-install"

if [ ! -d "$WASI_SDK_PREFIX" ]; then
    echo "Error: WASI SDK not found at $WASI_SDK_PREFIX"
    echo "Set WASI_SDK_PREFIX environment variable to the WASI SDK path"
    exit 1
fi

if [ ! -d "$ABSEIL_SOURCE" ]; then
    echo "Error: Abseil source not found at $ABSEIL_SOURCE"
    echo "Make sure abseil-cpp is in third_party/ or set ABSEIL_SOURCE"
    exit 1
fi

echo "Building Abseil for WASI..."
echo "  WASI SDK: $WASI_SDK_PREFIX"
echo "  Abseil source: $ABSEIL_SOURCE"
echo "  Build directory: $BUILD_DIR"
echo "  Install directory: $INSTALL_DIR"

# Configure
cmake -B "$BUILD_DIR" -S "$ABSEIL_SOURCE" \
    -DCMAKE_TOOLCHAIN_FILE="$SCRIPT_DIR/cmake/wasi-toolchain.cmake" \
    -DWASI_SDK_PREFIX="$WASI_SDK_PREFIX" \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=17 \
    -DABSL_PROPAGATE_CXX_STD=ON \
    -DABSL_BUILD_TESTING=OFF \
    -DABSL_USE_GOOGLETEST_HEAD=OFF \
    -DBUILD_SHARED_LIBS=OFF

# Build
cmake --build "$BUILD_DIR" --parallel

# Install
cmake --install "$BUILD_DIR"

echo ""
echo "Abseil for WASI built successfully!"
echo "Install location: $INSTALL_DIR"
echo ""
echo "Now run ./build-wasi.sh to build protoc for WASI"
