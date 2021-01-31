# Nsh
![build status](https://github.com/juliencombattelli/nsh/workflows/Build%20&%20Tests/badge.svg)

Nsh (NanoSHell) is a lightwheight shell targeting embedded systems.
It can also be run on Unix-like platforms mainly for testing purposes.

## Features

Nsh provides the following features:
- **Stack-only allocations** — Nsh does not allocate anything dynamically
- **Custom commands** — Nsh provides an help and an exit command by default, the user can register new ones at compile-time
- **Commands autocompletion** — Press the autocompletion key to start the autocomplete procedure
- **Commands history** — Nsh keeps track of the commands run in the current power-cycle (no persistency yet)
- **Return code printing** — Nsh can print the return code of the last run command (like Cygwin)
- **Optional features** — Almost all Nsh features can be disabled at compile-time if not wanted to save flash space

Nsh supports the following platforms:
- **Unix** — Tested on Linux, MacOS, MinGW
- **ST Nucleo F411RE** — other STM32 devices can be easily added, thanks to [ObKo/stm32-cmake](https://github.com/ObKo/stm32-cmake)

## Building

### Requirements

This project requires CMake 3.17+ to build the source code.
If building for STM32, a arm-none-eabi-gcc toolchain is required, OpenOCD and GDB are needed to flash and debug the target.
Here are some build procedure examples.

### Native Unix build in Debug mode

```bash
# Configure the project
cmake -S path-to-nsh -B nsh-build-native-debug -DCMAKE_BUILD_TYPE=Debug
# Build Nsh
cmake --build nsh-build-native-debug --parallel 4
# Run tests
cd nsh-build-native-debug
ctest
```

### ST Nucleo F411RE build

```bash
# Configure the project
cmake -S path-to-nsh -B nsh-build-f411re-release -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchains/Stm32Gcc.cmake -DSTM32_TOOLCHAIN_PATH=/tmp/gcc-arm-none-eabi -DNSH_STM32_TARGET=Stm32NucleoF411RE
# Build Nsh
cmake --build nsh-build-f411re-release --parallel 4
# Flash the target
cmake --build nsh-build-f411re-release --target simple_shell-flash
```

## Contributing

If you want to get involved and suggest some additional features, signal a bug or submit a patch, please create a pull request or open an issue on the [Nsh Github repository](https://github.com/juliencombattelli/nsh).
