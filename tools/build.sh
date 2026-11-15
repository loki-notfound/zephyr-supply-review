#!/usr/bin/env bash
# Builds the C++ tools of this directory against an existing build of ZephyrProtocol/zephyr v2.3.0.
#
#   tools/build.sh <zephyr-cmake-build-dir> [output-dir]
#
# <zephyr-cmake-build-dir> is the directory that holds CMakeCache.txt after a normal build of the
# daemon and its utilities (for "make release" that is build/<platform>/<branch>/release).
# The tools reuse the compile flags and the link line of zephyr-blockchain-stats, so nothing in
# the Zephyr tree is changed. Output: one executable per tool in [output-dir] (default: ./bin).
set -euo pipefail
build=${1:?usage: tools/build.sh <zephyr-cmake-build-dir> [output-dir]}
here=$(cd "$(dirname "$0")" && pwd)
out=$(mkdir -p "${2:-bin}" && cd "${2:-bin}" && pwd)
util="$build/src/blockchain_utilities"
dir="$util/CMakeFiles/blockchain_stats.dir"
[ -f "$dir/flags.make" ] && [ -f "$dir/link.txt" ] || { echo "no blockchain_stats build found under $build" >&2; exit 1; }
flags=$(grep -E '^CXX_(DEFINES|INCLUDES|FLAGS)' "$dir/flags.make" | sed 's/^[A-Z_]* = //' | tr '\n' ' ')
libs=$(sed 's/.*zephyr-blockchain-stats //' "$dir/link.txt")
cd "$util"
for src in "$here"/*.cpp; do
  name=$(basename "$src" .cpp)
  eval "c++ $flags -w -c '$src' -o '$out/$name.o'"
  eval "c++ -pthread -O2 -pie '$out/$name.o' -o '$out/$name' -Wl,--start-group $libs -Wl,--end-group"
  rm -f "$out/$name.o"
  echo "built $out/$name"
done
