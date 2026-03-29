# Changelog

All notable changes to this project will be documented in this file

---

## [UnReleased] - 03/29/2026

### Added

- Parser implementation foundation:
  - `src/parser/include/ast.h`: Complete AST node definitions with all statement and expression types
  - `src/parser/include/parser.h`: Parser public API and diagnostic structures
  - `src/parser/parser.cpp`: Minimal parser implementation (token streaming, error collection)
- Test file `tests/lexer/error_numeric_suffix.mc` covering numeric literal suffix validation corner cases

### Changed

- Lexer numeric suffix validation improved: Invalid tokens like `123main` or `.5test` are now consumed entirely as single Error tokens
  - `tokenizeNumber()`: When digit literal is followed by letter/underscore, consume entire invalid identifier sequence before returning Error token
  - `tokenizeLeadingDotFloat()`: When float literal is followed by letter/underscore, consume entire invalid identifier sequence before returning Error token
- Lexer include paths unified: All includes now use `#include "lexer/..."` and `#include "common/..."` format from src root
- `docs/design/lexer.md`: Added validation rules documentation for numeric literals
- Test framework `scripts/run_tests.py`: Changed to capture stderr for diagnostics instead of stdout

### Removed

- None

---

## [UnReleased] - 03/28/2026

### Added

- `docs/design/parser.md` completed with full parser design content (Sections 1-13)

### Changed

- Parser design document refined for implementation readiness:
  - fixed wording and naming inconsistencies
  - completed table of contents
  - milestone section standardized into table layout
  - extension points section standardized into table layout

### Removed

- None

---

## [UnReleased] - 03/27/2026

### Added

- `string` keyword support in lexer token model (`KeywordString`)
- Keyword test coverage updated to include `string`

### Changed

- `docs/specification.md` aligned with current syntax decisions:
  - delimiters now limited to `(` `)` `{` `}` `;` `,`
  - postfix expressions currently support function calls only
  - indexing (`[]`) and member access (`.`) are explicitly not supported yet
- `docs/design/lexer.md` incrementally aligned with specification updates:
  - keywords now include `string`
  - delimiter definition updated to remove `[` `]` `.`
  - future extension wording adjusted for numeric literal expansion
- Lexer keyword table updated to recognize `string` as a reserved keyword
- Exponent detection logic in `lexer.cpp` refactored for readability while preserving behavior

### Removed

- Removed `LBracket`, `RBracket`, and `Dot` delimiter tokens from lexer token model and lookup tables

---

## [UnReleased] - 03/20/2026

### Added

- Extended floating-point number format support:
  - Exponent notation: `1e5`, `1E-3`, `100e+2` (both uppercase and lowercase E)
  - Leading-dot literals: `.5`, `.5e-10` (floats starting with decimal point)
- Test coverage for exponent notation with leading zeros and sign variations
- Restructure the lexer files hierarchy. Make it more modular, move token and tables to their own files.

### Changed

- Refactored `tokenizeNumber()` and `nextToken()` to support exponent notation
- Extracted leading-dot float parsing into dedicated function for better maintainability
- Updated `lexer.md` with new floating-point literal grammar rules
- Improved code separation of concerns: dedicated tokenizer for each literal type

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

## [UnReleased] - 03/12/2026

### Added

- various diagrams added under `images/lexer`

### Changed
- update `lexer.md` to cover more topics, and state the ideas behind the lexer design

### Removed
- None

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


