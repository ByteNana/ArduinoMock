_src := "src/task.c src/queue.c src/semphr.c src/timers.c src/esp_log.c"
_sysroot_flag := if os() == "macos" { "--extra-arg=--sysroot=" + `xcrun --show-sdk-path` } else { "" }

# Run clang-tidy and report issues
lint: configure
    clang-tidy -p {{build_dir}} {{_sysroot_flag}} {{_src}}

# Run clang-tidy and exit non-zero on any warning (for CI)
lint-check: configure
    clang-tidy -p {{build_dir}} {{_sysroot_flag}} --warnings-as-errors='*' {{_src}}
