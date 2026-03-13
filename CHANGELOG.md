# Changelog

All notable changes to this project will be documented in this file

The format is based on **Keep a Changelog**
and this project adheres to **Semantic Versioning**

---

## [Not Released] - 03/10/2026

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

## [Not Released] - 03/11/2026

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

## [Not Released] - 03/12/2026

### Added

- various diagrams added under `images/lexer`

### Changed
- update `lexer.md` to cover more topics, and state the ideas behind the lexer design

### Removed
- None
