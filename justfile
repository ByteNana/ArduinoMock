build_dir := "build"

import '.just/build.just'
import '.just/test.just'
import '.just/lint.just'
import '.just/format.just'

# List all available recipes
default:
    @just --list
