# Build and run all test suites
test: build
    ctest --test-dir {{build_dir}} --output-on-failure

# Run a single suite by name: just test-suite test_queue
test-suite suite: build
    ctest --test-dir {{build_dir}} --output-on-failure -R {{suite}}
