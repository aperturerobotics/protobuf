#!/bin/bash
# Build protoc for WASI
# Run build-abseil-wasi.sh first to build Abseil dependencies

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
WASI_SDK_PREFIX="${WASI_SDK_PREFIX:-$HOME/repos/wasi-sdk-bin-29.0}"
BUILD_DIR="${SCRIPT_DIR}/build-wasi"
ABSEIL_INSTALL_DIR="${SCRIPT_DIR}/build-abseil-wasi-install"

if [ ! -d "$WASI_SDK_PREFIX" ]; then
    echo "Error: WASI SDK not found at $WASI_SDK_PREFIX"
    echo "Set WASI_SDK_PREFIX environment variable to the WASI SDK path"
    exit 1
fi

if [ ! -d "$ABSEIL_INSTALL_DIR" ]; then
    echo "Error: Abseil WASI build not found at $ABSEIL_INSTALL_DIR"
    echo "Run ./build-abseil-wasi.sh first"
    exit 1
fi

echo "Building protoc for WASI..."
echo "  WASI SDK: $WASI_SDK_PREFIX"
echo "  Build directory: $BUILD_DIR"
echo "  Abseil install: $ABSEIL_INSTALL_DIR"

# Configure
cmake -B "$BUILD_DIR" -S "$SCRIPT_DIR" \
    -DCMAKE_TOOLCHAIN_FILE="$SCRIPT_DIR/cmake/wasi-toolchain.cmake" \
    -DWASI_SDK_PREFIX="$WASI_SDK_PREFIX" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="$ABSEIL_INSTALL_DIR" \
    -Dabsl_DIR="$ABSEIL_INSTALL_DIR/lib/cmake/absl" \
    -Dprotobuf_BUILD_TESTS=OFF \
    -Dprotobuf_BUILD_CONFORMANCE=OFF \
    -Dprotobuf_BUILD_EXAMPLES=OFF \
    -Dprotobuf_BUILD_PROTOC_BINARIES=OFF \
    -Dprotobuf_BUILD_LIBUPB=OFF \
    -Dprotobuf_BUILD_WASI_REACTOR=ON \
    -Dprotobuf_WITH_ZLIB=OFF \
    -Dprotobuf_BUILD_SHARED_LIBS=OFF \
    -Dprotobuf_ABSL_PROVIDER=package

# Build the WASI reactor
cmake --build "$BUILD_DIR" --target protoc_wasi --parallel

# Show output
WASM_FILE="$BUILD_DIR/protoc.wasm"
if [ -f "$WASM_FILE" ]; then
    echo ""
    echo "protoc WASI reactor built successfully!"
    echo "Output: $WASM_FILE"
    echo "Size: $(du -h "$WASM_FILE" | cut -f1)"
else
    echo ""
    echo "Error: protoc.wasm not found at expected location"
    exit 1
fi
