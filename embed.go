// Package protobuf embeds the protobuf C++ library source files.
package protobuf

import "embed"

// CMakeFiles embeds CMakeLists.txt and cmake helper files needed for building.
//
//go:embed CMakeLists.txt
//go:embed cmake/*.cmake
//go:embed src/file_lists.cmake
var CMakeFiles embed.FS
