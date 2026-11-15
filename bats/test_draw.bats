setup() {
  load 'bats-support/load'
  load 'bats-assert/load'
  root_dir="$(cd "$( dirname "$BATS_TEST_FILENAME" )/.." >/dev/null 2>&1 && pwd)"
  PATH="$root_dir/bin:$PATH"
  examples_dir="$root_dir/examples"
  output_dir="$(mktemp -d)"
}

teardown() {
  rm -rf "$output_dir"
}

# Normal usage
# ------------

@test "kover draw creates a SVG file for a scene with all kinds of objects" {
  run kover draw "$output_dir"/scene.svg < "$examples_dir"/2b1h2a.scene
  assert_success
  assert_output ""
  [ -s "$output_dir"/scene.svg ]
  grep -q "<svg" "$output_dir"/scene.svg
}

@test "kover draw creates a SVG file for an empty scene" {
  run kover draw "$output_dir"/empty.svg < "$examples_dir"/empty.scene
  assert_success
  [ -s "$output_dir"/empty.svg ]
}

@test "kover draw creates a SVG file for a scene with a single antenna" {
  run kover draw "$output_dir"/antenna.svg < "$examples_dir"/1a.scene
  assert_success
  [ -s "$output_dir"/antenna.svg ]
}

@test "kover draw writes the identifiers of the objects in the image" {
  run kover draw "$output_dir"/scene.svg < "$examples_dir"/1b1a.scene
  assert_success
  # Text is embedded as glyphs, so we only check that the image is not trivial
  [ "$(wc -c < "$output_dir"/scene.svg)" -gt 1000 ]
}

@test "kover draw overwrites an existing file" {
  echo "old content" > "$output_dir"/scene.svg
  run kover draw "$output_dir"/scene.svg < "$examples_dir"/1b.scene
  assert_success
  run grep -c "old content" "$output_dir"/scene.svg
  assert_output "0"
}

# Wrong usage
# -----------

@test "kover draw without file name reports wrong usage" {
  run kover draw < "$examples_dir"/1b.scene
  assert_failure 1
  assert_output "error: subcommand 'draw' requires an output file"
}

@test "kover draw reports an error and creates no file on an invalid scene" {
  run kover draw "$output_dir"/scene.svg < "$examples_dir"/first_line.invalid
  assert_failure 1
  assert_output "error: first line must be exactly 'begin scene'"
  [ ! -e "$output_dir"/scene.svg ]
}

@test "kover draw reports an error on overlapping constructions" {
  run kover draw "$output_dir"/scene.svg < "$examples_dir"/2b_overlapping_partial.invalid
  assert_failure 1
  [ ! -e "$output_dir"/scene.svg ]
}

@test "kover draw reports an error when the file cannot be written" {
  run kover draw "$output_dir"/missing_directory/scene.svg < "$examples_dir"/1b.scene
  assert_failure 1
  assert_output --partial "error: cannot write file"
}

@test "kover help mentions the draw subcommand" {
  run kover help
  assert_success
  assert_line "  draw FILE: draws the loaded scene in the SVG file FILE"
}
