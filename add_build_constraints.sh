#!/bin/bash
cd /Users/cjs/repos/protobuf/src/google/protobuf

find . -type f \( -name "*.cc" -o -name "*.h" -o -name "*.inc" \) | while read f; do
  if [ -f "$f" ]; then
    first_line=$(head -1 "$f")
    if [ "$first_line" != "//go:build deps_only && cgo" ]; then
      grep -v "//go:build" "$f" > "$f.tmp"
      echo '//go:build deps_only && cgo' > "$f.new"
      echo '' >> "$f.new"
      cat "$f.tmp" >> "$f.new"
      mv "$f.new" "$f"
      rm -f "$f.tmp"
    fi
  fi
done

echo "Done"
