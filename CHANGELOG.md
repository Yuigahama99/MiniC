# Changelog

All notable changes to this project will be documented in this file

---

## [UnReleased] - 03/10/2026

### Added

- Initial project documentation structure.
- `README.MD` with project overview.
- `docs/specification.md` defining the MiniC language.
- `docs/architecture.md` describing compiler architecture.
- `docs/roadmap.md` outlining development phases.
- Update project directory structure to include:
    - `docs/design/` for detailed design documents
    - `src/` modules for lexer, parser, semantic analysis, IR, optimizer, coodegen, runtime.
    - `tests/` modules corresponding to each compiler stage.
- Added optimizer design and src structure.
- Added runtime test structure.

### Changed
- None.

### Removed
- None.

---

## [UnReleased] - 03/11/2026

### Added

- Lexer implementaion to cover minimal MiniC Requirement
- `lexer.h` and `lexer.cpp` added under `src\lexer`
- Now MiniC lexer should be able to read the source and tokenize all the elements
- Error token will be generated with line and column number of the start location
- One simple test `.mc` file added under `test\lexer`
- Update `.gitignore` to ignore `.vscode`

### Changed
- `lexer.md` structure changed, typo fixed, content aligned with actual implementation
- `roadmap.md` Milestone Summary aligned with actual progress

### Removed
- roadmap.png, use raw MD table instead in roadmap.md

---

## [UnReleased] - 03/12/2026

### Added

- various diagrams added under `images/lexer`

### Changed
- update `lexer.md` to cover more topics, and state the ideas behind the lexer design

### Removed
- None

---

## [UnReleased] - 03/13/2026

### Added

- Test suites added under `tests/lexer/`
- scripts under `src/scripts` that handle automated testing
- MAKEFILE with various make options
- DEBUG_LOG() interface

### Changed
- `main.cpp` will run lexer now
- `docs/roadmap.md` rewrite, mark lexer done
- `README.md` rewrite, make it prettier
- `.gitignore` covers build output, log output and test output
- Debug logs added in lexer code
- bugfix: lexer dead loop when some error token corner case encountered
- Refactor nextToken() to separate unknown token handling from tokenizeOperator()

### Removed
- None

---

## [0.1.0] - 03/17/2026

### Added

- `CMakeLists.txt` added as main build entry for MiniC
- `CMakePresets.json` added with `llvm-debug` and `llvm-release`
- CMake custom targets added for `test` and `test-lexer`
- New `src/common/` module added to host shared utilities (`debug.h`, `debug.cpp`)
- MIT `LICENSE` file added for project release

### Changed

- Project build workflow migrated to CMake + official LLVM clang on macOS
- `Makefile` refactored into a CMake wrapper (`all`, `debug`, `test`, `test-lexer`, `clean`)
- debug module moved from `src/` to `src/common/`
- include paths updated to unix style (`/`) and new module path (`common/debug.h`)
- `CMakeLists.txt` updated to build `src/common/debug.cpp`
- `run_tests.py` updated to use `build/minic` binary in macOS workflow
- `README.md` rewritten for CMake/LLVM usage and updated project structure section

### Removed

- Windows-specific build flow and commands (`mingw32-make`, `if not exist`, `rmdir /s /q`)
- Windows binary naming usage (`minic.exe`) in test workflow
- old debug module location under `src/debug.h` and `src/debug.cpp`