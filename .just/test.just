_suites := "test_task test_queue test_semphr test_timers"

# Build and run all test suites
test: build
    #!/usr/bin/env bash
    set -euo pipefail
    failed=0
    for suite in {{_suites}}; do
        {{build_dir}}/test/$suite || failed=1
    done
    exit $failed

# Run a single suite by name: just test-suite test_queue
test-suite suite: build
    {{build_dir}}/test/{{suite}}
