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
- **Makefile Build System** — One-command build with `make` / `make debug` / `make clean`
- **Debug Logging** — Compile-time toggleable (`-DENABLE_DEBUG_LOG`), per-module log files (`logs/lexer.log`, etc.), printf-style `DEBUG_LOG` macro and RAII `TRACE_ENTER_EXIT` for automatic function tracing
- **Automated Test Runner** — Python-based, auto-discovers test suites and cases, outputs results to `tests/<suite>/output/`

### 📐 Design Principles
- **Modular Architecture** — Each compiler phase lives in its own directory with independent headers
- **Extensible** — Adding a new compiler phase only requires a new module directory and Makefile entry

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
├── src/
│   ├── main.cpp            # Entry point
│   ├── debug.h / debug.cpp # Debug logging system
│   ├── lexer/              # Lexer module
│   ├── parser/             # Parser module (WIP)
│   ├── semantic/           # Semantic analysis (WIP)
│   ├── ir/                 # Intermediate representation (WIP)
│   ├── optimizer/          # Optimization passes (WIP)
│   ├── codegen/            # Code generation (WIP)
│   └── runtime/            # Runtime / VM (WIP)
├── tests/
│   └── lexer/              # Lexer test cases (.mc files)
├── scripts/
│   ├── run_tests.py        # Test runner (all suites)
│   └── test_lexer.py       # Lexer-only test wrapper
├── docs/                   # Documentation
└── Makefile                # Build system
```

## 🚀 Getting Started

### Prerequisites

- **C++17 compiler** (g++ 13+ recommended)
- **Make** (`mingw32-make` on Windows / MSYS2)
- **Python 3** (for running tests)

### Build

```bash
# Normal build (no debug logging)
mingw32-make

# Build with debug logging enabled
mingw32-make debug

# Clean build artifacts
mingw32-make clean
```

### Run

```bash
./build/minic <source_file.mc>
```

### Test

```bash
# Run all test suites
mingw32-make test

# Run all test suites with log enabled
mingw32-make debug test

# Run lexer tests only
mingw32-make test-lexer
```

Test output is saved to `tests/<suite>/output/*.out` for inspection.

## 🐛 Debug Logging

The debug logging system can be toggled at compile time:

- `mingw32-make` — logging disabled (zero overhead)
- `mingw32-make debug` — logging enabled, logs written to `logs/<module>.log`

Usage in code:

```cpp
#include "debug.h"

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

TBD