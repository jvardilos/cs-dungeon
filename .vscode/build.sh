#!/usr/bin/env bash
# Build one entry-point .cpp plus the implementation files of every local header
# it (transitively) includes: #include "foo.h" -> foo.cpp next to foo.h.
# Usage: build.sh <source.cpp> <output-binary>
set -euo pipefail

src=$1
out=$2

sources=("$src")
i=0
while [ "$i" -lt "${#sources[@]}" ]; do
    f=${sources[$i]}
    i=$((i + 1))
    dir=$(dirname "$f")
    # quoted includes only; <system> headers are ignored
    for inc in $(sed -n 's/^[[:space:]]*#[[:space:]]*include[[:space:]]*"\([^"]*\)".*/\1/p' "$f"); do
        hdr="$dir/$inc"
        [ -f "$hdr" ] || continue
        impl="${hdr%.*}.cpp"
        [ -f "$impl" ] || continue
        case " ${sources[*]} " in *" $impl "*) continue ;; esac
        sources+=("$impl")
    done
done

mkdir -p "$(dirname "$out")"
echo "building: ${sources[*]}" >&2
exec clang++ -std=c++20 -g -O0 -Wall -Wextra -fcolor-diagnostics -fansi-escape-codes \
    "${sources[@]}" -o "$out"
