_files := "src/*.c test/*.c include/freertos/*.h include/*.h"

# Format all C source and header files in-place
format:
    clang-format -i {{_files}}

# Check formatting without modifying files (for CI)
format-check:
    clang-format --dry-run -Werror {{_files}}
