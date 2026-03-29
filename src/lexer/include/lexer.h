#pragma once

#include "lexer/include/token.h"

#include <cstddef>
#include <string>
#include <optional>

// ----------------------------------
// Lexer class
// ----------------------------------
class Lexer
{
public:
    explicit Lexer(std::string source);

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
    Token tokenizeLeadingDotFloat();
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