# MiniC Lexer Design

The lexer (lexical analyzer) is the first stage of the MiniC compiler.
Its job is to read the raw source code and convert it into a stream of tokens for the parser.

---

## Table of Contents
- [Overview](#1-overview)
- [Token Types](#2-token-types)
- [Lexing Rules](#3-lexing-rules)
- [Error Handling](#4-error-handling)
- [Token Structure](#5-token-structure)
- [Lexer API](#6-lexer-api)
- [Implementation Notes](#7-implementation-notes)
- [Example](#8-example)
- [Future Extensions](#9-future-extensions)
- [Quick Test Checklist](#10-quick-test-checklist)

---

## 1. Overview

The lexer performs the following tasks:

- Reads characters from the input source
- Groups characters into meaningful uints (tokens)
- Skip whitespace and comments
- Reports lexical errors (invalid characters, malformed literals)
- Tracks line and column information for error reporting

The lexer does **not** handle syntax or semantic rules; it only recognizes the smallest meaningful units of the language

---

## 2. Token Types

MiniC tokens fall into several categories:

### 2.1 Keywords
`int` `float` `char` `void`
`if` `else` `while` `for` `return`

### 2.2 Identifiers

- Start with a letter or underscore
- Followed by letters, digits, or underscores

### 2.3 Literals

- Interger literals (e.g. `123`)
- Floating-point literals (e.g. `1.23`)
- Charater literals (e.g. `'a'`)
- String literals (e.g. `"hello"`)

### 2.4 Operators
`+` `-` `*` `/` `%`
`=` `==` `!=`
`<` `>` `<=` `>=`
`&&` `||` `!`

### 2.5 Delimiters
`()` `{}` `[]` `;` `,` `.`

### 2.6 Commenrs

- Line comment: `// ...`
- Block comment: `/* ... */`

Comments are ignored by the lexer

---

## 3. Lexing Rules

### 3.1 Whitespace

Whitespace charaters (`' '`, `'\t'`, `'\n'`, `'\r`) are skipped.

### 3.2 Identifiers and Keywords

- If the first characters is a letter or `_`, scan util a non-alphanumeric character.
- Check if the resulting string is a keyword; otherwise clasify as an identifier.

### 3.3 Numbers

- If the first character is a digit, scan digits
- If a dot `.` follows, scan fractional part (optional for MiniC)

### 3.4 Strings and Characters

- Strings begin and end with `"`.
- Characters begin and end with `'`.
- Escape sequence (Optional), currently not supported.

### 3.5 Operators

Operators maybe:

- Single-character (`+`, `-`, `*`, `/`)
- Multi-character (`==`, `!=`, `<=`, `>=`, `&&`, `||`)

Use lookahead to distingguish multi-character operators.

### 3.6 Comments

- `//` consumes until end of line
- `/* ... */` consumes until closing `*/`
- Unterminated block comments are errors

---

## 4. Error Handling

The lexer reports errors such as:

- Invalid characters
- Unterminated string or character literal
- Unterminated block comment
- Invalid escape sequences (Optional), currently no escape sequences support.

Errors include line an column information for the starting loctaion

---

## 5. Token Structure

Each token contains:

- `type` - token type enum
- `lexeme` - the exact text
- `line` - line number
- `column` - column number
- `value` - parsed literal value (optional)

Example:
```json
{
    "type": "IntLiteral",
    "lexeme": "123",
    "line": 4,
    "column": 10 
}
```

---

## 6. Lexer API

The lexer exposes:

- `nextToken()` - returns the next token
- `peekToken()` - (optional)
- `reset()` - (optional)

The parser repeatedly calls `nextToken()` until `EOF`

---

## 7. Implementation Notes

- Use a pointer or index to track the current character
- Maintain `line` and `column` counters
- Use helper functions:
    - `peek()`
    - `peekNext()`
    - `advance()`

---

## 8. Example

Source:
```
int main()
{
    return 42;
}
```

Token stream:
```
Keyword(int)
Identifier(main)
LParen
RParen
LBrace
Keyword(return)
IntLiteral(42)
Semicolon
RBrace
EOF
```

---

## 9. Future extensions

| **Extension** | **Priority** | **Benefit** | **Complexity** | **Notes** |
|---|---:|---|---:|---|
| Escape sequence support | High | Makes string/char literals usable and compatible with common code | Medium | Decode common escapes; add `decodedValue` to `Token` |
| Exponent and leading-dot floats | Medium | More complete numeric literal support | Low | Support `1e-3`, `3.14E+2`, and `.5` (route `.`+digit to number parser) |
| Decoded literal value field | Medium | Parser/AST can use ready-to-use literal values | Low | Keep `lexeme` raw; add optional `decodedValue` for literals |
| Unterminated comment/string diagnostics | High | Better error messages and tooling UX | Low | Ensure start line/column recorded before consuming `/*`, `"`, `'` |
| Unicode identifiers | Low | Internationalization and modern identifier support | High | Requires Unicode-aware identifier classification (UTF-8 codepoint handling) |
| Hex/octal/binary integer literals | Low | More literal formats for systems programming | Medium | Support `0x`, `0o`, `0b` prefixes and validation |
| Unicode/UTF-8 string escapes | Low | Support `\u`/`\U` escapes for portability | High | Implement `\uXXXX`/`\UXXXXXXXX` decoding and validation |
| Peek/reset API and token buffering | Medium | Parser lookahead and backtracking support | Low | Add `peekToken()` and `reset()` or a small token buffer |
| Configurable lexer modes | Low | Allow language dialects or feature flags (e.g., strict mode) | Medium | Feature flags to toggle escapes, numeric formats, Unicode, etc. |


---

## 10. Quick Test Checklist

- **Comments**
    - `/* untermianed` -> error at comment start
    - `/* comment */` -> skip comment
    - `// comment\nnext` -> skip comment, return `next` token
- **Literals**
    - `'a'` -> char literal
    - `'\n'` -> error (escape not supported)
    - `"hello"` -> string literal
    - `"hello\nworld"` -> error (escape not supported)
- **Numbers**
    - `42` -> int literal
    - `3.14` -> floating point literal
- **Operators**
    - `==`, `!=`, `&&`, `||` recognized
- **Errors**
    - All errors include `line` and `column` and short context