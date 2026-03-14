#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

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

// --------------------------------
// Keyword mapping
// --------------------------------
static const std::unordered_map<std::string, TokenType> keywords = {
    {"int", TokenType::KeywordInt},
    {"float", TokenType::KeywordFloat},
    {"char", TokenType::KeywordChar},
    {"void", TokenType::KeywordVoid},
    {"if", TokenType::KeywordIf},
    {"else", TokenType::KeywordElse},
    {"while", TokenType::KeywordWhile},
    {"for", TokenType::KeywordFor},
    {"return", TokenType::KeywordReturn},
};

// --------------------------------
// Operator mapping
// --------------------------------
static const std::unordered_map<std::string, TokenType> operators = {
    // arithmetic operators
    {"+", TokenType::Plus},
    {"-", TokenType::Minus},
    {"*", TokenType::Multiply},
    {"/", TokenType::Divide},
    {"%", TokenType::Modulo},

    // assignment operator
    {"=", TokenType::Assign},

    // comparison operators
    {"==", TokenType::Equal},
    {"!=", TokenType::NotEqual},
    {"<", TokenType::Less},
    {"<=", TokenType::LessEqual},
    {">", TokenType::Greater},
    {">=", TokenType::GreaterEqual},

    // logical operators
    {"&&", TokenType::And},
    {"||", TokenType::Or},
    {"!", TokenType::Not},
};

// --------------------------------
// Delimiter mapping
// --------------------------------
static const std::unordered_map<char, TokenType> delimiters = {
    {'(', TokenType::LParen},
    {')', TokenType::RParen},
    {'{', TokenType::LBrace},
    {'}', TokenType::RBrace},
    {'[', TokenType::LBracket},
    {']', TokenType::RBracket},
    {';', TokenType::Semicolon},
    {',', TokenType::Comma},
    {'.', TokenType::Dot},
};

// ----------------------------------
// Lexer class
// ----------------------------------
class Lexer
{
public:
    Lexer(const std::string &source);

    // get the next token
    Token nextToken();

private:
    // source handling
    char peek() const;
    char peekNext() const;
    char advance();

    // source cleaning
    std::optional<Token> skipWhitespaceOrComment();

    // tokenization
    Token tokenizeIdentifier();
    Token tokenizeNumber();
    Token tokenizeChar();
    Token tokenizeString();
    Token tokenizeOperator(const std::string &lexeme);
    Token tokenizeDelimiter();

private:
    const std::string source;
    size_t current = 0;
    int line = 1;
    int column = 1;
};