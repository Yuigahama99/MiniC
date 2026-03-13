# Design Guide for MiniC Lexer

This document provides a concise overview of the MiniC lexer design. It explains the design choices behind the lexer and the reasoning for adopting the current workflow. Common lexer is built using a standard pipeline: defining token rules with regular expressions, converting them into NFAs, and then transforming those NFAs into DFAs for efficient and deterministic scanning.

**Note: The current MiniC lexer is handwritten and not generated automatically from a DFA, although its control logic is fundamentally based on DFA principles.**

---

## Table of Contents
1. [Common Lexer Implementation Pipelines](#1-common-lexer-implementation-pipelines)
   - [1.1 Regex -> NFA -> DFA](#11-regex---nfa---dfa)
   - [1.2 Direct NFA Execution](#12-direct-nfa-execution)
   - [1.3 Regex-Based Matching Without Automata](#13-regex-based-matching-without-automata)
   - [1.4 Parsing Expression Grammars (PEGs)](#14-parsing-expression-grammars-pegs)
2. [Token Definitions with Regular Expressions](#2-token-definitions-with-regular-expressions)
   - [2.1 Keywords](#21-keywords)
   - [2.2 Identifiers](#22-identifiers)
   - [2.3 Literals](#23-literals)
     - [Integer Literals](#integer-literals)
     - [Floating-Point Literals](#floating-point-literals)
     - [Character Literals](#character-literals)
     - [String Literals](#string-literals)
   - [2.4 Operators](#24-operators)
     - [Arithmetic Operators](#arithmetic-operators)
     - [Relational Operators](#relational-operators)
     - [Logical Operators](#logical-operators)
     - [Assignment Operator](#assignment-operator)
   - [2.5 Delimiters](#25-delimiters)
   - [2.6 Comments](#26-comments)
     - [Line Comments](#line-comments)
     - [Block Comments](#block-comments)
   - [2.7 Whitespace](#27-whitespace)
3. [Regex to NFA](#3-regex-to-nfa)
   - [3.1 Overview of Thompson's Construction](#31-overview-of-thompsons-construction)
   - [3.2 NFA Fragments for Basic Components](#32-nfa-fragments-for-basic-components)
     - [Literal Characters](#literal-characters)
     - [Concatenation](#concatenation)
     - [Alternation](#alternation)
     - [Kleene Star](#kleene-star)
   - [3.3 Examples of Converting MiniC's Regex to NFA](#33-examples-of-converting-minics-regex-to-nfa)
     - [Keywords](#keywords)
     - [Identifiers](#identifiers)
4. [NFA to DFA](#4-nfa-to-dfa)
   - [4.1 Overview of Subset Construction](#41-overview-of-subset-construction)
   - [4.2 Subset Construction Algorithm](#42-subset-construction-algorithm)
   - [4.3 Examples of Converting MiniC's NFA to DFA](#43-examples-of-converting-minics-nfa-to-dfa)
5. [Generating Code from DFA](#5-generating-code-from-dfa)
   - [5.1 Representing the DFA](#51-representing-the-dfa)
   - [5.2 Handwritten Lexer Implementation](#52-handwritten-lexer-implementation)
6. [Future Extensions](#6-future-extensions)

---

## 1. Common Lexer Implementation Pipelines

Lexers can be implemented using various pipelines, depending on the requirements of the language and the constraints of the implementation. This section outlines the most common pipelines for lexer design, including their advantages and disadvantages.

---

### 1.1 Regex -> NFA -> DFA

This is the most widely used pipeline in modern lexer design. It involves the following steps:

#### Workflow：

1. **Define token rules using regular expressions**: Regular expressions are used to describe the patterns for tokens.
2. **Convert regular expressions into NFAs**: Using algorithms like Thompson's construction, each regex is converted into an NFA.
3. **Transform NFAs into a single DFA**: The subset construction algorithm is used to eliminate non-determinism and produce a deterministic finite automaton.

#### Examples:

- **Lex/Flex**: These tools use this pipeline to generate efficient DFA-based lexers from regular expressions.
- **GCC (GNU Compiler Collection)**: The lexer in GCC is generated using a DFA-based approach, making it highly efficient for large-scale C programs.
- **Clang**: The Clang lexer also uses a DFA-based implementation for tokenization.

#### Advantages:

- **Efficiency**: DFA-based lexers process input in linear time, as each character requires a single state transition.
- **Automation**: Tools like Lex/Flex automate the conversion process, reducing development effort.
- **Scalability**: Well-suited for large inputs and complex token patterns.

#### Disadvantages:

- **State explosion**: The NFA-to-DFA conversion can lead to an exponential increase in the number of states for complex regular expressions.
- **Limited context sensitivity**: DFA-based lexers cannot handle context-sensitive rules without additional mechanisms.

---

### 1.2 Direct NFA Execution

Instead of converting an NFA into a DFA, this pipeline directly executes the NFA to recognize tokens. The NFA is constructed from regular expressions, but no deterministic automaton is generated.

#### Workflow:

1. **Define token rules using regular expressions**.
2. **Convert regular expressions into NFAs**.
3. **Directly execute the NFA**: The lexer simulates the NFA at runtime, using backtracking or ε-closure computation to handle non-determinism.

#### Examples:

- **RE2 (Google's Regular Expression Engine)**: RE2 uses a hybrid approach that avoids full DFA construction and directly executes NFAs for certain patterns.
- **Some scripting languages**: Interpreted languages like Python or Ruby may use direct NFA execution for their regex engines.

#### Advantages:

- **Avoids state explosion**: Since no DFA is generated, the memory overhead of state explosion is avoided.
- **Simplicity**: The implementation is simpler, as it skips the NFA-to-DFA conversion step.

#### Disadvantages:

- **Performance**: Direct NFA execution is slower than DFA-based lexers, as it may require backtracking or ε-closure computation.
- **Non-deterministic runtime**: The execution time depends on the complexity of the NFA and the input.

---

### 1.3 Regex-Based Matching Without Automata

This pipeline skips the automata construction entirely and directly uses regular expression engines to match tokens.

#### Workflow:

1. **Define token rules using regular expressions**.
2. **Use a regex engine to match tokens**: The lexer uses a library or built-in regex engine to find matches in the input string.

#### Examples:

- **Python's `re` module**: Python's built-in regex engine is often used for simple tokenization tasks.
- **Java's `Pattern` and `Matcher` classes**: These classes provide regex-based matching for tokenization in Java applications.
- **Small DSLs**: Domain-specific languages (DSLs) often use regex engines for quick and simple lexers.

#### Advantages:

- **Ease of implementation**: No need to manually construct or manage automata.
- **Flexibility**: Modern regex engines support advanced features like lookahead and backreferences.

#### Disadvantages:

- **Performance**: Regex engines are typically slower than DFA-based lexers for large inputs, as they may involve backtracking.
- **Dependency on libraries**: Relies on the performance and correctness of the regex engine.

---

### 1.4 Parsing Expression Grammars (PEGs)

PEGs are an alternative to regular expressions that unify lexical and syntactic analysis. In this pipeline, the lexer and parser are combined into a single stage.

#### Workflow:

1. **Define token and grammar rules using PEGs**.
2. **Use a PEG parser to tokenize and parse simultaneously**.

#### Examples:

- **Rust's `nom` library**: A PEG-based parser combinator library that can handle both lexical and syntactic analysis.
- **Packrat parsers**: Many PEG-based implementations use packrat parsing to ensure linear-time performance.
- **Modern language tools**: Some modern languages like Julia use PEGs for their parsing infrastructure.

#### Advantages:

- **Context sensitivity**: PEGs can handle context-sensitive rules, making them more powerful than regular expressions.
- **Unified framework**: Combines lexical and syntactic analysis, reducing the need for separate lexer and parser stages.

#### Disadvantages:

- **Complexity**: PEGs are harder to understand and implement compared to regular expressions.
- **Performance**: PEG-based lexers are generally slower than DFA-based lexers for purely lexical tasks.

---

### Summary of Pipelines

| Pipeline                    | Examples                              | Advantages                           | Disadvantages                                |
|-----------------------------|---------------------------------------|--------------------------------------|----------------------------------------------|
| Regex -> NFA -> DFA         | Lex/Flex, GCC, Clang                  | Efficient, automated, scalable       | State explosion, limited context sensitivity |
| Direct NFA Execution        | RE2, Python/Ruby regex engines        | Avoids state explosion, simple       | Slower, non-deterministic                    |
| Regex-Based Matching        | Python `re`, Java `Pattern`/`Matcher` | Easy to implement, flexible          | Slower, depends on regex engine              |
| Parsing Expression Grammars | Rust `nom`, Packrat parsers           | Context-sensitive, unified framework | Complex, slower for lexical tasks            |

---

## 2. Token Definitions with Regular Expressions

This section defines the regular expressions used to recognize the tokens in MiniC. Each oken type corresponds to a specific pattern, which is described below.

---

### 2.1 keywords

Keywords are reserved words in MiniC. They are matched exactly as written:
`int` `float` `char` `void`
`if` `else` `while` `for` `return`

```regex
\b(int|float|char|void|if|else|while|for|return)\b
```

---

### 2.2 identifiers

Identifiers are user-defined names for variables, functions, etc. They must start with a letter or underscore and can be followed by letters, digits, or underscores.

```regex
[a-zA-Z_][a-ZA-Z0-9_]*
```

---

### 2.3 Literals

#### Interger Literals

Interger literals are sequences of digits.

```regex
[0-9]+
```

#### Floating-Point Literals

Floating-point literals consist of digits, followed by a dot (`.`), and more digitis.

```regex
[0-9]+\.[0-9]+
```

#### Charater Literals

Character literals are single characters enclosed in single quotes (`'`).

```regex
'[^']'
```

#### String Literals

String literals are sequences of characters enclosed in double quotes (`"`). Escape sequence are currently not supported in MiniC, will extend in the future.

```regex
"[^"]*"
```

---

### 2.4 Operators

Operators in MiniC include arithmetic, logical, and assignment operators

#### Arithmetic Operators

```regex
(\+|\-|\*|\/|%)
```

#### Relational Operators:

```regex
(==|!=|<=|>=|<|>)
```

#### Logical Operators:

```regex
(&&|\|\||!)
```

#### Assignment Operator:

```regex
=
```

---

### 2.5 Delimiters

Delimiters includes parentheses, braces, brackets, semicolons, commas, and dots

```regex
(\(|\)|\{|\}|\[|\]|;|,|\.)
```

---

### 2.6 Comments

#### Line Comments

Line comments start with `//` and continue until th ened of the line

```regex
\/\/.*
```

#### Block Comments

Block comments start with `/*` and end with `*/`. They can span multiple lines.

```regex
\/\*[\s\S]*?\*\/
```

---

### 2.7 Whitesoace

Whitespace characters include spaces, tabs, newlines, and carriage returns. These are ignored by the lexer.

```regex
[ \t\r\n]+
```

---

## 3. Regex to NFA

This section explains how regular expressions are converted into Non-deterministic Finite Automata (NFA). The process is based on **Thompson's Construction Algorithm**, which systematically builds an NFA for any given regular expression.

---

### 3.1 Overview of Thompson's Construction

Thompson's Construction is a method for converting a regular expression into an NFA. The key idea is to break regex into smaller components and build NFA fragments for each component. These fragments are then combined to form the final NFA.

#### Basic Components:

1. **Literal Characters**: A single character (e.g., `a`)
2. **Concatenation**: Two pattens in sequence (e.g., `ab`)
3. **Alternation**: A choice between two pattens (e.g., `a|b`)
4. **Kleene Star**: Zero or more repetitions of a pattern (e.g., `a*`)

---

### 3.2 NFA Fragments for Basic Components

#### Literal Characters

A literal character (e.g., `a`) is represented as an NFA with two states and one transition.

![literal character](../../images/lexerDesign/literal_char_nfa.png)

#### Concatenation

For two pattens `A` and `B`, the NFA for `AB` connects the accept state of `A` to the start state of `B`.

![concatenation](../../images/lexerDesign/concatenated_nfa.png)

#### Alternation

For two patterns `A` and `B`, the NFA for `A|B` introduces a new start state with ε-transitions to the start states of `A` and `B`.

![alternation](../../images/lexerDesign/alternation_nfa.png)

#### Kleene Star

For a pattern `A`, the NFA for `A*` introduces a new start state with ε-transitions to the start state of `A` and directly to the accept state.

![kleene start](../../images/lexerDesign/kleene_star_nfa.png)

---

### 3.3 Examples of Converting MiniC's Regex to NFA

#### Keywords

**Regex**

```regex
\b(int|float|char|void|if|else|while|for|return)\b
```

**NFA**

![keywords](../../images/lexerDesign/keywords_nfa.png)

#### Identifiers

**Regex**

```regex
[a-zA-Z_][a-ZA-Z0-9_]*
```

**NFA**

![identifiers](../../images/lexerDesign/identifiers_nfa.png)

---

## 4. NFA to DFA

This section explains how to convert a Non-deterministic Finite Automaton (NFA) into a Deterministic Finite Automaton (DFA). The process is based on the **Subset Construction Algorithm**, which systematically eliminates non-determinism by grouping NFA states into DFA states.

---

### 4.1 Overview of Subset Construction

The Subset Construction Algorithm works by:

1. Treating each DFA state as a **set of NFA states**.
2. Computing transitions for each DFA state based on the union of transitions from the corresponding NFA states.
3. Iteratively constructing new DFA states until all reachable states are processed.

#### Key Concepts:

- **ε-closure**: The set of all NFA states reachable from a given state (or set of states) via ε-transitions.
- **Deterministic Transitions**: Each DFA state has exactly one transition for each input symbol.

---

### 4.2 Subset Construction Algorithm

#### Algorithm Steps:

1. **Start State**:
   - Compute the ε-closure of the NFA's start state. This becomes the DFA's start state.

2. **Transitions**:
   - For each DFA state and each input symbol:
     - Compute the set of NFA states reachable via the input symbol.
     - Compute the ε-closure of the resulting states.
     - Add this set as a new DFA state if it hasn't been processed yet.

3. **Accept States**:
   - Any DFA state that contains at least one NFA accept state becomes a DFA accept state.

4. **Repeat**:
   - Repeat the process until no new DFA states are added.

---

### 4.3 Examples of Converting MiniC's NFA to DFA

#### Keywords

**NFA**

![keywords](../../images/lexerDesign/keywords_nfa.png)

**Steps**

1. Start State:
   - Compute the ε-closure of the NFA's start state.
   - The ε-closure includes all states reachable via ε-transitions:
     `ε-closure(Start) = {Start, key1, key2, key3, key4, key5, key6, key7, key8, key9}`
   - this becomes the DFA's start state: `{Start, key1, key2, key3, key4, key5, key6, key7, key8, key9}`.
2. Transition
   - For each input symbol `(i, f, c, etc.)`, compute the transitions from the current DFA state:
     - On i: Move to `{i2, i5}` (the next state in the `int` and `if` NFAs).
     - On f: Move to `{f2, f8}` (the next state in the `float` and `for` NFAs).
     - On c: Move to `{c3}` (the next state in the `char` NFA).
     - On v: Move to `{v4}` (the next state in the `void` NFA).
     - On e: Move to `{e6}` (the next state in the `else` NFA).
     - On w: Move to `{w7}` (the next state in the `while` NFA).
     - On r: Move to `{r9}` (the next state in the `return` NFA).
3. Repeat:
   - For each new DFA state, compute transitions for all input symbols.
   - Add new DFA states as needed until all reachable states are processed.
4. Accept States:
   - Any DFA state that contains an NFA accept state becomes a DFA accept state.

**DFA**
![keywords](../../images/lexerDesign/keywords_dfa.png)

---

## 5. Generating Code from DFA

This section explains how to convert the constructed DFA into executable code for the MiniC lexer. The DFA is implemented as state machine in code, where each state corresponds to a specific set of NFA states, and transitions are represented as a table or confitional logic.

---

### 5.1 Representing the DFA

The DFA can be represented in code using the following components:

1. State Enumeration:
   - Each DFA state is assigned a unique identifier (e.g., STATE_0, STATE_1, etc.).
   - Accept states are marked with their corresponding token type (e.g., TOKEN_INT, TOKEN_IF).

2. Transition Table:
   - A 2D array or dictionary is used to represent transitions between states.
   - Rows represent DFA states, and columns represent input symbols.
   - Each cell contains the next state for a given input symbol.

3. Accept States:
   - A separate data structure (e.g., a set or array) is used to store the accept states and their corresponding token types.

---

### 5.2 Handwritten Lexer Implementation

While the DFA construction process is a common approach for building efficient lexers, the current MiniC lexer is handwritten. This means that instead of relying on an automatically generated DFA, the lexer uses manually written logic to tokenize the input.

**How**
The MiniC lexer is implemented as a state machine, but instead of using a precomputed DFA, it uses a combination of:

- Conditionals: `if` and `switch` statements to determine the next state based on the current character.
- Helper Functions: Functions like `tokenizeIdentifier`, `tokenizeNumber`, and `tokenizeOperator` are used to handle specific token types.
- Manual State Management: The lexer uses methods like `peek` and `advance` to manually manage the input stream and determine the next state.

**Comparison with DFA-Based Lexer**

| Feature        | Handwritten Lexer                        | DFA-Based Lexer                   |
|----------------|------------------------------------------|-----------------------------------|
| Implementation | Manually written logic and conditionals. | Automatically generated from DFA. |
| Flexibility        | High: Can handle context-sensitive rules. | Limited: Requires additional           | mechanisms. |
| Performance        | Slightly slower for large inputs.         | Highly optimized for speed.            |
| Development Effort | Higher: Requires manual state management. | Lower: DFA is generated automatically. |
| Error Handling     | Fully customizable.                       | Limited to predefined patterns.        |

---

## 6. Future Extensions

| **Extension**                              | **Priority** | **Benefit**                                                       | **Complexity** | **Notes**                                                                                                           |
|--------------------------------------------|-------------:|-------------------------------------------------------------------|---------------:|---------------------------------------------------------------------------------------------------------------------|
| Escape sequence support                    |         High | Makes string/char literals usable and compatible with common code |         Medium | Decode common escapes; add `decodedValue` to `Token`                                                                |
| Exponent and leading-dot floats            |       Medium | More complete numeric literal support                             |            Low | Support `1e-3`, `3.14E+2`, and `.5` (route `.`+digit to number parser)                                              |
| Decoded literal value field                |       Medium | Parser/AST can use ready-to-use literal values                    |            Low | Keep `lexeme` raw; add optional `decodedValue` for literals                                                         |
| Unterminated comment/string diagnostics    |         High | Better error messages and tooling UX                              |            Low | Ensure start line/column recorded before consuming `/*`, `"`, `'`                                                   |
| Unicode identifiers                        |          Low | Internationalization and modern identifier support                |           High | Requires Unicode-aware identifier classification (UTF-8 codepoint handling)                                         |
| Hex/octal/binary integer literals          |          Low | More literal formats for systems programming                      |         Medium | Support `0x`, `0o`, `0b` prefixes and validation                                                                    |
| Unicode/UTF-8 string escapes               |          Low | Support `\u`/`\U` escapes for portability                         |           High | Implement `\uXXXX`/`\UXXXXXXXX` decoding and validation                                                             |
| Peek/reset API and token buffering         |       Medium | Parser lookahead and backtracking support                         |            Low | Add `peekToken()` and `reset()` or a small token buffer                                                             |
| Configurable lexer modes                   |          Low | Allow language dialects or feature flags (e.g., strict mode)      |         Medium | Feature flags to toggle escapes, numeric formats, Unicode, etc.                                                     |
| Explore lexer auto-gen tools like Lex/Flex |         High | Align with industry                                               |            Low | Tools allow you to define tokenization rules using regular expressions and automatically generate a DFA-based lexer |
