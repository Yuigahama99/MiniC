# 🛠️ MiniC Compiler

A lightweight C-like compiler built from scratch in C++17 for learning and experimenting with compiler construction.

---

## 📖 Overview

MiniC is a minimal subset of C designed for learning purposes. This project implements a complete compiler toolchain — from lexical analysis to (eventually) code generation — with a clean, modular architecture that is easy to follow and extend.

## ✨ Features

### 🏗️ Compiler Pipeline
- **Lexer** — Full tokenizer supporting keywords, identifiers, int/float/char/string literals, multi-char operators, and delimiters
- **Parser** / **Semantic** / **IR** / **Optimizer** / **Codegen** — Coming soon

### 🧰 Tooling & Infrastructure
- **CMake Build System** — LLVM clang-based build with `cmake --preset ...`
- **Debug Logging** — Compile-time toggleable (`-DENABLE_DEBUG_LOG`), per-module log files (`logs/lexer.log`, etc.), printf-style `DEBUG_LOG` macro and RAII `TRACE_ENTER_EXIT` for automatic function tracing
- **Automated Test Runner** — Python-based, auto-discovers test suites and cases, outputs results to `tests/<suite>/output/`

### 📐 Design Principles
- **Modular Architecture** — Each compiler phase lives in its own directory with independent headers
- **Extensible** — Adding a new compiler phase only requires a new module directory and CMake target entry

## 🚧 Roadmap

| Module       | Status         |
|--------------|----------------|
| Lexer        | ✅ Complete     |
| Parser       | 🔲 Planned      |
| Semantic     | 🔲 Planned      |
| IR           | 🔲 Planned      |
| Optimizer    | 🔲 Planned      |
| Code Gen     | 🔲 Planned      |
| Runtime / VM | 🔲 Planned      |

## 📂 Project Structure

```
MiniC/
├── src/                          # Compiler source code
│   ├── main.cpp                  # Compiler entry point
│   ├── common/                   # Shared utilities (planned)
│   │   └── debug.h / debug.cpp   # Logging and trace helpers
│   ├── lexer/                    # Lexer module (implemented)
│   │   ├── include/              # Public lexer headers
│   │   │   ├── token.h           # Token and TokenType definitions
│   │   │   ├── lexer.h           # Lexer class interface
│   │   │   └── lexer_tables.h    # Lexer lookup table interfaces
│   │   ├── lexer.cpp             # Lexer implementation
│   │   └── lexer_tables.cpp      # Keyword/operator/delimiter lookup tables
│   ├── parser/                   # Parser module (planned)
│   ├── semantic/                 # Semantic analysis module (planned)
│   ├── ir/                       # IR module (planned)
│   ├── optimizer/                # Optimization passes (planned)
│   ├── codegen/                  # Code generation module (planned)
│   └── runtime/                  # Runtime/VM module (planned)
├── tests/                        # Test inputs by module
│   └── lexer/                    # Lexer test cases (.mc)
├── scripts/
│   ├── run_tests.py              # Test runner (all suites)
│   └── test_lexer.py             # Lexer-only wrapper
├── docs/                         # Design/spec docs
├── CMakeLists.txt                # Main CMake build definition
├── CMakePresets.json             # LLVM debug/release presets
├── Makefile                      # Convenience wrapper for CMake
└── README.md
```

## 🚀 Getting Started

### Prerequisites

- **CMake** (3.20+)
- **LLVM clang** (Homebrew `llvm`)
- **Python 3** (for running tests)

### Build

```bash
# Configure + build (LLVM clang debug)
cmake --preset llvm-debug
cmake --build --preset build-debug

# Build with debug logging enabled
cmake --preset llvm-debug -DENABLE_DEBUG_LOG=ON
cmake --build --preset build-debug

# Clean build artifacts
rm -rf build logs
```

### Run

```bash
./build/minic <source_file.mc>
```

### Test

```bash
# Run all test suites
cmake --build build --target test

# Run all test suites with log enabled
cmake --preset llvm-debug -DENABLE_DEBUG_LOG=ON
cmake --build --preset build-debug
cmake --build build --target test

# Run lexer tests only
cmake --build build --target test-lexer
```

Test output is saved to `tests/<suite>/output/*.out` for inspection.

## 🐛 Debug Logging

The debug logging system can be toggled at compile time:

- `cmake --preset llvm-debug && cmake --build --preset build-debug` — logging disabled (zero overhead)
- `cmake --preset llvm-debug -DENABLE_DEBUG_LOG=ON && cmake --build --preset build-debug` — logging enabled, logs written to `logs/<module>.log`

Usage in code:

```cpp
#include "common/debug.h"

void someFunction() {
    TRACE_ENTER_EXIT(LogModule::Lexer);  // Auto logs Entering/Exiting
    DEBUG_LOG(LogModule::Lexer, LogLevel::Info, "Processing token: %s", lexeme.c_str());
}
```

Log levels: `Debug`, `Info`, `Warn`, `Error`

## 📚 Documentation

| Document | Description |
|----------|-------------|
| [specification.md](docs/specification.md) | MiniC language specification |
| [architecture.md](docs/architecture.md) | Compiler architecture overview |
| [roadmap.md](docs/roadmap.md) | Development roadmap |
| [design/](docs/design/) | Per-module design documents |

## 📄 License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.