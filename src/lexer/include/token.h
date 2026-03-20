#pragma once

#include <string>

// ----------------------------------
// Token types
// ----------------------------------
enum class TokenType
{
    // keywords
    KeywordInt,
    KeywordFloat,
    KeywordChar,
    KeywordVoid,
    KeywordIf,
    KeywordElse,
    KeywordWhile,
    KeywordFor,
    KeywordReturn,

    // identifiers and literals
    Identifier,
    IntLiteral,
    FloatLiteral,
    CharLiteral,
    StringLiteral,

    // operators
    Plus,
    Minus,
    Multiply,
    Divide,
    Modulo,
    Assign,
    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    And,
    Or,
    Not,

    // delimiters
    LParen,
    RParen,
    LBrace,
    RBrace,
    LBracket,
    RBracket,
    Semicolon,
    Comma,
    Dot,

    // special
    EndOfFile,
    Error
};

// ----------------------------------
// Token structure
// ----------------------------------
struct Token
{
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token(TokenType type, std::string lexeme, int line, int column)
        : type(type), lexeme(std::move(lexeme)), line(line), column(column) {}
};
