# Configure CMake and generate compile_commands.json
configure:
    cmake -S . -B {{build_dir}} -DBUILD_TESTING=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build the library and all test binaries
build: configure
    cmake --build {{build_dir}} --parallel

# Remove all build artifacts
clean:
    rm -rf {{build_dir}}
