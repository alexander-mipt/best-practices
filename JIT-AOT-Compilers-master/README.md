# Huawei 2022 JIT & AOT Compilers Course

### Dependencies

Install dependencies by running `sudo ./scripts/install_deps.sh`

### Build

Run:
- `git submodule update --init --recursive`
- `mkdir build && cd build`
- `cmake -DCMAKE_BUILD_TYPE=Debug $PATH_TO_SOURCES_ROOT_DIR && make`

After build `build/bin` directory will contain two executables: one with tests and another with a program building an IR graph for factorial computation function.
