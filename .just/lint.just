_src := "src/task.c src/queue.c src/semphr.c src/timers.c src/esp_log.c"

# Run clang-tidy and report issues
lint: configure
    clang-tidy -p {{build_dir}} {{_src}}

# Run clang-tidy and exit non-zero on any warning (for CI)
lint-check: configure
    clang-tidy -p {{build_dir}} --warnings-as-errors='*' {{_src}}
