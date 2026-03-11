# MiniC Language Specification

This document defines the syntax, semantics, and core features of the MiniC programming language.
MiniC is designed as a compact, educational subset of the C family, suitable for compiler construction and language experimentation.

---

## 1. Design Goals

MiniC aims to:

- Provide a minimal yet expressive C-like language
- Maintain deterministic and easy-to-parse grammer
- Support essential programming constructs without unnecessary complexity
- Serve as a foundation for building a full compiler toolchain
- Enable experimentation with parsing, semantic analysis, IR and code generation

---

## 2. Lexical Structure

### 2.1 Character Set
MiniC uses the ASCII character set.

### 2.2 Identifiers
Identifiers consist of:

- Letters (`A-Z`, `a-z`)
- Digits (`0`-`9`)
- Underscore (`_`)

Identifiers must not begin with a digits.

Examples:
`x`
`counter`
`_temp`
`value2`

### 2.3 Keywords
The following keywords are reserved:
`int`
`float`
`char`
void`
`if`
`else`
`while`
`return`

### 2.4 Literals

#### Interger Literals
Decimal integers:
`0`
`42`
`1234`

#### Floating-Point Literals
`3.14`
`0.5`
`10.0`

#### Character Literals
`'a'`
`'\n'`
`'0'`

### 2.5 Operators
`+` `-` `*` `/` `%`
`=` `==` `!=` `<` `>` `<=` `>=`
`&&` `||`
`!`

### 2.6 Comments
- Single-line: `// comment`
- Multi-line: `/* comment */`

---

## 3. Types
MiniC supports the following primitive types:
`int`
`float` 
`char`
`void`

No pointers, arrays, or structs are included in teh base language (optional extension may be added later).

---

## 4. Expressions

### 4.1 Primary Expressions
- Identifiers
- Literals
- Parenthesized expressions

### 4.2 Unary Expressions
`!expr`
`-expr`
`+expr`

### 4.3 Binary Expressions
Supported binary operatiors include:
- Arithemetic: `+ - * / %`
- Comparison: `== != < > <= >=`
- Logical: `&& ||`

### 4.4 Assignment
`Identifier = expression`
Assignments are expressions and return the assigned value.

---

## 5. Statements

### 5.1 Expression Statement
`expresion;`

### 5.2 Compound Statement (Block)
`{ statement* }`

### 5.3 Variable Declaration
`type identifier;`
`type identifier = expression;`

### 5.4 If Statement
`if(expression) statement`
`if(expression) statement else statement`

### 5.5 While Statement
`while(expression) statement`

### 5.6 Return Statement
`return ;`
`return expression;`

---

## 6. Functions

### 6.1 Function Declaration
`type identifier(parameter-list) compound-statement`

### 6.2 Parameters
`type identifier`

Multiple parameters are comma-separated.

Examples:
`int add(int a, int b) {`
`return a + b;`
`}`

---

## 7. Program Structure

A MiniC program consists of one or more function definitions.
The entry point is the function:

`int main()`

---

## 8. Grammar (EBNF)

`program = function-definition*`

`function-definition = type identifier"("parameter-list?")" compound-statement`

`parameter-list = parameter("," parameter)*`
`parameter = type identifier`

`type = "int" | "float" | "char" | "void"`

`compound-statement = { statement }`

`statement = expression-statement`
`| declaration`
`| if-statement`
`| while-statement`
`| return-statement`
`| compound-statement`

`expression-statement = expression?";"`

`declaration = type identifier ("=" expresion)?";"`

`if-statement = "if""("expression")" statement "(""else" statement")"?`

`while-statement = "while""("expression")" statement`

`return-statement = "retur" expression?";"`

`expression = assignment`

`assignment = identifier "=" assignment | logical-or`

`logical-or = logical-and ("||" logical-and)*`
`logical-and = equality ("&&" equality)*`
`equality = relational (("==" | "!=") relational)*`
`relational = additive (("<" | ">" | "<=" | ">=") additive)*`
`additive = multiplicative (("+" | "-") multiplicative)*`
`multiplicative = unary (("*" | "/" | "%") unary)`
`unary = ("!" | "-" | "+") unary | primary`

`primary = identifier`
`| literal`
`| "(" expression ")"`

`literal = interger-literal`
`| float-literal`
`| char-literal`

---

## 9. Future Extensions (Optional)

Possible future additions:

- Arrays
- Strings
- Structs
- Functions returning non-primitive types
- Type inference
- Modules or namespaces
- A standard library

These are not part of the core MiniC specification but may be added as the project evolves.

---

## 10. Summary

MiniC is intentionally small, consistent, and easy to parse.
This specification defines the core language features required to build a complete compiler pipeline, while leaving room for future exapnsion.