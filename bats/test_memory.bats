setup() {
  load 'bats-support/load'
  load 'bats-assert/load'
  root_dir="$(cd "$( dirname "$BATS_TEST_FILENAME" )/.." >/dev/null 2>&1 && pwd)"
  PATH="$root_dir/bin:$PATH"
  examples_dir="$root_dir/examples"
  valgrind="valgrind --leak-check=full --error-exitcode=1"
  command -v valgrind > /dev/null || skip "valgrind is not installed"
  output_dir="$(mktemp -d)"
}

teardown() {
  rm -rf "$output_dir"
}

@test "kover bounding-box handles memory correctly on a given scene" {
  $valgrind kover bounding-box < "$examples_dir"/3b2a.scene
}

@test "kover describe handles memory correctly on a given scene" {
  $valgrind kover describe < "$examples_dir"/3b2a.scene
}

@test "kover draw handles memory correctly on a given scene" {
  $valgrind kover draw "$output_dir"/scene.svg < "$examples_dir"/2b1h2a.scene
}

@test "kover quality handles memory correctly on a given scene" {
  $valgrind kover quality < "$examples_dir"/1b1a_3corners.scene
}

@test "kover summarize handles memory correctly on a given scene" {
  $valgrind kover summarize < "$examples_dir"/3b2a.scene
}

@test "kover validate handles memory correctly on a given scene" {
  $valgrind kover validate < "$examples_dir"/3b2a.scene
}

@test "kover handles memory correctly on an empty scene" {
  $valgrind kover describe < "$examples_dir"/empty.scene
}
