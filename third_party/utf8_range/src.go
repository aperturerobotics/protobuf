package utf8_range

import "embed"

// Source embeds the utf8_range C++ source files for vendoring.
//
//go:embed *.c *.cc *.h *.inc
var Source embed.FS
