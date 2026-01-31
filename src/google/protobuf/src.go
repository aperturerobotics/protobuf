package protobuf

import "embed"

// Source embeds the Protobuf C++ source files for vendoring.
//
//go:embed *.cc *.h *.inc
//go:embed stubs/*.cc stubs/*.h
//go:embed io/*.cc io/*.h
//go:embed util/*.cc util/*.h
//go:embed json/*.cc json/*.h
//go:embed json/internal/*.cc json/internal/*.h
//go:embed compiler/*.cc compiler/*.h
//go:embed compiler/*/*.cc compiler/*/*.h
var Source embed.FS
