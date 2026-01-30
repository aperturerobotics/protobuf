package protobuf

import "embed"

// Source embeds the C++ protobuf runtime source files for vendoring.
// Only includes files needed by generated .pb.cc/.pb.h files.
//
//go:embed *.cc *.h *.proto *.inc
//go:embed stubs/*.cc stubs/*.h
//go:embed io/*.cc io/*.h
var Source embed.FS
