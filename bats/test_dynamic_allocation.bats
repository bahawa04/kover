setup() {
  load 'bats-support/load'
  load 'bats-assert/load'
  root_dir="$(cd "$( dirname "$BATS_TEST_FILENAME" )/.." >/dev/null 2>&1 && pwd)"
  PATH="$root_dir/bin:$PATH"
  scene_file="$(mktemp)"
}

teardown() {
  rm -f "$scene_file"
}

# Generates a scene with $1 buildings and $2 antennas, placed on a grid
function generate_scene {
  echo "begin scene" > "$scene_file"
  for ((i = 0; i < $1; ++i)); do
    echo "  building b$i $((i * 4)) 0 1 1" >> "$scene_file"
  done
  for ((i = 0; i < $2; ++i)); do
    echo "  antenna a$i $((i * 4)) 10 2" >> "$scene_file"
  done
  echo "end scene" >> "$scene_file"
}

# Scenes larger than the former fixed capacity (100 objects)
# -----------------------------------------------------------

@test "kover summarize handles more than 100 buildings and antennas" {
  generate_scene 250 300
  run kover summarize < "$scene_file"
  assert_success
  assert_output "A scene with 250 buildings and 300 antennas"
}

@test "kover validate handles a large scene" {
  generate_scene 150 150
  run kover validate < "$scene_file"
  assert_success
  assert_output "ok"
}

@test "kover bounding-box handles a large scene" {
  generate_scene 120 0
  run kover bounding-box < "$scene_file"
  assert_success
  assert_output "bounding box [-1, 477] x [-1, 1]"
}

@test "kover describe lists objects in identifier order whatever the input order" {
  printf 'begin scene\n  building b2 10 0 1 1\n  building b10 20 0 1 1\n  building b1 0 0 1 1\nend scene\n' > "$scene_file"
  run kover describe < "$scene_file"
  assert_success
  assert_output --partial "  building b1 at 0 0 with dimensions 1 1"
  assert_line --index 2 "  building b10 at 20 0 with dimensions 1 1"
  assert_line --index 3 "  building b2 at 10 0 with dimensions 1 1"
}

# Limits of the input
# -------------------

@test "kover validate accepts an identifier of 10 characters" {
  printf 'begin scene\n  building abcdefghij 0 0 1 1\nend scene\n' > "$scene_file"
  run kover validate < "$scene_file"
  assert_success
  assert_output "ok"
}

@test "kover validate rejects an identifier of 11 characters" {
  printf 'begin scene\n  building abcdefghijk 0 0 1 1\nend scene\n' > "$scene_file"
  run kover validate < "$scene_file"
  assert_failure 1
  assert_output --partial "not ok"
  assert_output --partial 'error: invalid identifier "abcdefghijk" (line #2)'
}

@test "kover validate rejects a line with too many tokens without crashing" {
  printf 'begin scene\n  building b 0 0 1 1 1 1 1 1 1 1 1\nend scene\n' > "$scene_file"
  run kover validate < "$scene_file"
  assert_failure 1
  assert_output --partial "error: building line has wrong number of arguments (line #2)"
}

@test "kover validate rejects a line that is too long" {
  { echo "begin scene"; printf '  building b 0 0 1 1%100s\n' ""; echo "end scene"; } > "$scene_file"
  run kover validate < "$scene_file"
  assert_failure 1
  assert_output --partial "error: line is too long (line #2)"
}

@test "kover validate rejects an integer that does not fit in an int" {
  printf 'begin scene\n  building b 99999999999 0 1 1\nend scene\n' > "$scene_file"
  run kover validate < "$scene_file"
  assert_failure 1
  assert_output --partial 'error: invalid integer "99999999999" (line #2)'
}

@test "kover validate accepts tabs as blank characters" {
  printf 'begin scene\n\tbuilding\tb\t0 0\t1 1\t\nend scene\n' > "$scene_file"
  run kover validate < "$scene_file"
  assert_success
  assert_output "ok"
}

@test "kover reports an error on an empty line without printing not ok outside validate" {
  printf 'begin scene\n\nend scene\n' > "$scene_file"
  run kover summarize < "$scene_file"
  assert_failure 1
  assert_output "error: line has no token (line #2)"
}

@test "kover handles extreme coordinates without overflow" {
  printf 'begin scene\n  building b1 2147483647 2147483647 2147483647 2147483647\n  building b2 -2147483647 -2147483647 1 1\nend scene\n' > "$scene_file"
  run kover validate < "$scene_file"
  assert_success
  assert_output "ok"
}
