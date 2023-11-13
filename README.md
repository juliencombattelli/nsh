# Nsh

![build status](https://github.com/juliencombattelli/nsh/workflows/Build%20&%20Tests/badge.svg)
[![codecov](https://codecov.io/gh/juliencombattelli/nsh/branch/main/graph/badge.svg?token=0L5KEeuCMn)](https://codecov.io/gh/juliencombattelli/nsh)

Nsh (short for Nanoshell) is a portable lightweight shell. Written in C11, it
can be used on a large panel of platforms, from bare-metal targets to Unix
environments. Its small memory footprint makes it ideal for constrained embedded
systems.

## Features

Nsh provides the following features:
- **No allocation** — Nsh does not allocate anything by itself and let the user decide how objects should be instantiated
- **Custom commands** — Nsh provides an help and an exit command by default, the user can register new ones at compile-time
- **Hardware/OS agnostic** — Nsh provides interfaces the user can implement to integrate the shell into a specific platform
- **Commands autocompletion** — Press the autocompletion key to start the autocomplete procedure
- **Commands history** — Nsh keeps track of the commands run in the current power-cycle (no persistency yet)
- **Return code printing** — Nsh can print the return code of the last run command (like Cygwin)
- **Optional features** — Almost all Nsh features can be disabled at compile-time if not wanted to reduce program size

Nsh currently supports the following platforms:
- **Unix** — Tested on Linux, MacOS, MinGW
- **ST Nucleo F411RE** — other STM32 devices can be easily added, thanks to [ObKo/stm32-cmake](https://github.com/ObKo/stm32-cmake)

## Building

### Requirements

This project requires CMake 3.17+ to build the source code.
If building for the supported STM32 targets, an arm-none-eabi-gcc toolchain is
required, OpenOCD and GDB are needed to flash and debug the target.
Here are some build procedure examples.

### Native Unix build in Debug mode with test coverage

```bash
# Configure the project
cmake -S path-to-nsh -B nsh-build-native-debug -D CMAKE_BUILD_TYPE=Debug -D ENABLE_COVERAGE=ON
# Build Nsh
cmake --build nsh-build-native-debug --parallel 4
# Run tests
cmake --build nsh-build-native-debug --target test
# Run coverage report generation
cmake --build nsh-build-native-debug --target coverage
```

### ST Nucleo F411RE build

```bash
# Configure the project
cmake -S path-to-nsh -B nsh-build-f411re-release -D CMAKE_BUILD_TYPE=Release -D CMAKE_TOOLCHAIN_FILE=cmake/Toolchains/Stm32Gcc.cmake -D NSH_PLATFORM_NAME=Stm32NucleoF411RE
# Build Nsh
cmake --build nsh-build-f411re-release --parallel 4
# Flash the target
cmake --build nsh-build-f411re-release --target utests-flash
```

### Testing in QEMU virtual device

Nsh can run in a QEMU emulator supporting the ST Nucleo F411 board.
Such port can be found at https://github.com/juliencombattelli/qemu-st-nucleo-f411.
To use it, download and build QEMU:
```bash
git clone https://github.com/juliencombattelli/qemu-st-nucleo-f411
mkdir qemu-st-nucleo-f411-build
cd qemu-st-nucleo-f411-build
../qemu-st-nucleo-f411/configure
make
```

Then run the Nsh unit-test binary using the freshly built qemu-system-arm:
```bash
./qemu-system-arm -machine st-nucleo-f411 -kernel path/to/utests.bin -nographic -monitor none -serial null -serial stdio
```

> **Note:** Nsh unit-test binary uses UART2 for serial communications, so we
> have to explicitly use mon:stdio on the second serial argument to redirect the
> messages to the console.

## Contributing

If you want to get involved and suggest some additional features, signal a bug
or submit a patch, please create a pull request or open an issue on the
[Nsh Github repository](https://github.com/juliencombattelli/nsh).

### Git hooks

Nsh repository comes with git hooks useful during development.
To use them, run the following command:
```bash
git config core.hooksPath .githooks
```
### Formatting

Nsh uses automatic formatting using clang-format (version 12 at least).
Please run it before submitting a patch.
