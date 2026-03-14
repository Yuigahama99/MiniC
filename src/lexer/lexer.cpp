#include "include\lexer.h"
#include "..\debug.h"

// ----------------------------------
// helper functions
// ----------------------------------
/**
 * @brief Checks if a character is an alphabetic letter, wrapping std::isalpha to avoid signed char issues
 *
 * @param [char] c the character to check
 * @return [bool] true if the character is an alphabetic letter
 * @return [bool] false otherwise
 */
inline bool isAlpha(char c)
{
    return std::isalpha(static_cast<unsigned char>(c));
}

/**
 * @brief Checks if a character is a digit, wrapping std::isdigit to avoid signed char issues
 *
 * @param [char] c the character to check
 * @return [bool] true if the character is a digit
 * @return [bool] false otherwise
 */
inline bool isDigit(char c)
{
    return std::isdigit(static_cast<unsigned char>(c));
}

/**
 * @brief Checks if a character is alphanumeric, wrapping std::isalnum to avoid signed char issues
 *
 * @param [char] c the character to check
 * @return [bool] true if the character is alphanumeric
 * @return [bool] false otherwise
 */
inline bool isAlphaNum(char c)
{
    return std::isalnum(static_cast<unsigned char>(c));
}

// ----------------------------------
// Constructor for lexer
// ----------------------------------
Lexer::Lexer(const std::string &src)
    : source(src), current(0), line(1), column(1) {}

// ----------------------------------
// Source handling
// ----------------------------------
/**
 * @brief Peeks at the current character in the source without advancing
 *
 * @return [char] the current character, or '\0' if at the end of the source
 */
char Lexer::peek() const
{
    TRACE_ENTER_EXIT(LogModule::Lexer);

    if (current >= source.size())
    {
        DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "peek() at end of source, returning '\\0'");
        return '\0';
    }

    DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "peek() returning %s", charRepr(source[current]));
    return source[current];
}

/**
 * @brief Peeks at the next character in the source without advancing
 *
 * @return [char] the next character, or '\0' if at the end of the source
 */
char Lexer::peekNext() const
{
    TRACE_ENTER_EXIT(LogModule::Lexer);

    if (current + 1 >= source.size())
    {
        DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "peekNext() at end of source, returning '\\0'");
        return '\0';
    }

    DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "peekNext() returning %s", charRepr(source[current + 1]));
    return source[current + 1];
}

/**
 * @brief Advances the current position in the source and returns the character that was advanced over
 *
 * @return [char] the character that was advanced over, or '\0' if at the end of the source
 */
char Lexer::advance()
{
    TRACE_ENTER_EXIT(LogModule::Lexer);

    char c = peek();

    if (c == '\n')
    {
        line++;
        column = 1;
    }
    else
    {
        column++;
    }

    DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "cursor moved to line %d, column %d", line, column);
    current++;

    DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "advance() returning %s", charRepr(c));
    return c;
}

// ----------------------------------
// Source cleaning
// ----------------------------------
/**
 * @brief Skips over whitespace characters in the source, as well as comments (both line comments and block comments)
 *
 * @return std::optional<Token>
 */
std::optional<Token> Lexer::skipWhitespaceOrComment()
{
    TRACE_ENTER_EXIT(LogModule::Lexer);

    while (true)
    {
        char c = peek();

        switch (c)
        {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Skipping whitespace character %s", charRepr(c));
            advance();
            break;
        case '/':
            // check for comments
            if (peekNext() == '/')
            {
                DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "line comment detected, skipping until end of line");

                // line comment, consume '//'
                advance();
                advance();

                // skip until end of line or end of file
                while (peek() != '\n' && peek() != '\0')
                {
                    advance();
                }
            }
            else if (peekNext() == '*')
            {
                DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "block comment detected, skipping until \"*/\"");

                int startLine = line;
                int startColumn = column;

                // block comment, consume '/*'
                advance();
                advance();

                // skip until '*/' or end of file
                while (!(peek() == '*' && peekNext() == '/') && peek() != '\0')
                {
                    advance();
                }

                // consume '*/' if we found it
                if (peek() == '*' && peekNext() == '/')
                {
                    DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "End of block comment found");
                    advance();
                    advance();
                }
                else
                {
                    // Unterminated block comment
                    // EOF reached, no need to advance for this error
                    DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Unterminated block comment detected");
                    return Token(TokenType::Error, "Unterminated block comment", startLine, startColumn);
                }
            }
            else
            {
                DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Not a comment, stop skipping");
                return std::nullopt; // not a comment, stop skipping
            }
            break;

        default:
            DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Not whitespace or comment, stop skipping");
            return std::nullopt; // no more whitespace or comments
        }
    }
}

// ----------------------------------
// Tokenization
// ----------------------------------
/**
 * @brief Tokenizes an identifier or keyword from the source
 *
 * @return [Token] a token representing either an identifier or a keyword, depending on the lexeme
 */
Token Lexer::tokenizeIdentifier()
{
    TRACE_ENTER_EXIT(LogModule::Lexer);

    int startLine = line;
    int startColumn = column;
    size_t start = current;

    // consume the first character (letter or '_')
    advance();

    // consume the rest of the identifier (letters, digits, or '_')
    while (isAlphaNum(peek()) || peek() == '_')
    {
        advance();
    }

    // extract lexeme
    std::string lexeme = source.substr(start, current - start);

    // check if the lexeme is a keyword
    if (keywords.count(lexeme))
    {
        DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Keyword detected: %s", lexeme.c_str());
        return Token(keywords.at(lexeme), lexeme, startLine, startColumn);
    }

    // otherwise, it's an identifier
    DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Identifier detected: %s", lexeme.c_str());
    return Token(TokenType::Identifier, lexeme, startLine, startColumn);
}

/**
 * @brief Tokenizes a number (integer or float) from the source
 *
 * @return [Token] a token representing either an integer literal or a float literal, depending on the lexeme
 */
Token Lexer::tokenizeNumber()
{
    TRACE_ENTER_EXIT(LogModule::Lexer);

    int startLine = line;
    int startColumn = column;
    size_t start = current;

    // integer part
    while (isDigit(peek()))
    {
        advance(); // consume the integer part
    }

    // fractional part
    bool isFloat = false;
    if (peek() == '.' && isDigit(peekNext()))
    {
        DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Decimal point detected, tokenizing as float literal");
        isFloat = true;
        advance(); // consume '.'

        while (isDigit(peek()))
        {
            advance(); // consume the fractional part
        }
    }

    // extract lexeme
    std::string lexeme = source.substr(start, current - start);

    if (isFloat)
    {
        DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Float literal detected: %s", lexeme.c_str());
        return Token(TokenType::FloatLiteral, lexeme, startLine, startColumn);
    }

    DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Integer literal detected: %s", lexeme.c_str());
    return Token(TokenType::IntLiteral, lexeme, startLine, startColumn);
}

/**
 * @brief Tokenizes a character literal from the source
 *
 * @return [Token] a token representing a character literal
 */
Token Lexer::tokenizeChar()
{
    TRACE_ENTER_EXIT(LogModule::Lexer);

    int startLine = line;
    int startColumn = column;
    size_t start = current;

    // consume opening quote
    advance();

    char c = peek();
    if (c == '\0' || c == '\n')
    {
        // Unterminated char literal
        // EOF reached, no need to advance for this error
        // '\n' reached, will be consumed by skipWhitespaceOrComment upon next call to nextToken
        DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Unterminated char literal detected");
        return Token(TokenType::Error, "Unterminated char literal", startLine, startColumn);
    }

    // escapes currently not supported for MiniC, reject
    if (c == '\\')
    {
        DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Escape sequence detected, not supported in char literals");
        advance();
        return Token(TokenType::Error, "Escape sequences not supported in char literals", startLine, startColumn);
    }

    // consume the character
    advance();

    // consume closing quote
    if (peek() != '\'')
    {
        // Unterminated char literal
        DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Unterminated char literal detected");
        return Token(TokenType::Error, "Unterminated char literal", startLine, startColumn);
    }

    // consume closing quote
    advance();

    // extract lexeme
    std::string lexeme = source.substr(start, current - start);

    DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Char literal detected: %s", lexeme.c_str());
    return Token(TokenType::CharLiteral, lexeme, startLine, startColumn);
}

/**
 * @brief Tokenizes a string literal from the source
 *
 * @return [Token] a token representing a string literal
 */
Token Lexer::tokenizeString()
{
    TRACE_ENTER_EXIT(LogModule::Lexer);

    int startLine = line;
    int startColumn = column;
    size_t start = current;

    // consume opening double quote
    advance();

    while (true)
    {
        char c = peek();

        if (c == '\0' || c == '\n')
        {
            // Unterminated string literal
            // EOF reached, no need to advance for this error
            // '\n' reached, will be consumed by skipWhitespaceOrComment upon next call to nextToken
            DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Unterminated string literal detected");
            return Token(TokenType::Error, "Unterminated string literal", startLine, startColumn);
        }

        if (c == '"')
        {
            // consume closing double quote
            DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "End of string literal detected");
            advance();
            break; // end of string literal
        }

        // escapes currently not supported for MiniC, reject
        if (c == '\\')
        {
            DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Escape sequence detected, not supported in string literals");

            advance();
            return Token(TokenType::Error, "Escape sequences not supported in string literals", startLine, startColumn);
        }

        // consume character
        advance();
    }

    // extract lexeme
    std::string lexeme = source.substr(start, current - start);

    DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "String literal detected: %s", lexeme.c_str());
    return Token(TokenType::StringLiteral, lexeme, startLine, startColumn);
}

/**
 * @brief Tokenizes an operator from the source
 *
 * @return [Token] a token representing an operator
 */
Token Lexer::tokenizeOperator(const std::string &lexeme)
{
    TRACE_ENTER_EXIT(LogModule::Lexer);

    int startLine = line;
    int startColumn = column;

    for (size_t i = 0; i < lexeme.size(); i++)
        advance();

    DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Operator detected: %s", lexeme.c_str());
    return Token(operators.at(lexeme), lexeme, startLine, startColumn);
}

/**
 * @brief Tokenizes a delimiter from the source
 *
 * @return [Token] a token representing a delimiter
 */
Token Lexer::tokenizeDelimiter()
{
    TRACE_ENTER_EXIT(LogModule::Lexer);

    int startLine = line;
    int startColumn = column;
    size_t start = current;

    char c = advance();
    DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Delimiter detected: %s", charRepr(c));
    return Token(delimiters.at(c), source.substr(start, 1), startLine, startColumn);
}

/**
 * @brief Get the next token from the source
 *
 * @return [Token] the next token, or an error token if an invalid token is encountered
 */
Token Lexer::nextToken()
{
    TRACE_ENTER_EXIT(LogModule::Lexer);

    // 1. skip whitespace and comments
    auto err = skipWhitespaceOrComment();
    if (err.has_value())
        return *err;

    int startLine = line;
    int startColumn = column;
    char c = peek();

    DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Next token starts with: %s", charRepr(c));

    // 2. end of file
    if (c == '\0')
    {
        DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Reaching end of file");
        return Token(TokenType::EndOfFile, "", startLine, startColumn);
    }

    // 3. identifiers and keywords
    if (isAlpha(c) || c == '_')
    {
        DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "%s is a valid start of an identifier, tokenizing as identifier or keyword", charRepr(c));
        return tokenizeIdentifier();
    }

    // 4. numeric literals
    if (isDigit(c))
    {
        DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "%s is a valid start of a numeric literal, tokenizing as number", charRepr(c));
        return tokenizeNumber();
    }

    // 5. character literals
    if (c == '\'')
    {
        DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "%s is a valid start of a char literal, tokenizing as char literal", charRepr(c));
        return tokenizeChar();
    }

    // 6. string literals
    if (c == '"')
    {
        DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "%s is a valid start of a string literal, tokenizing as string literal", charRepr(c));
        return tokenizeString();
    }

    // 7. delimiters
    if (delimiters.count(c))
    {
        DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "%s is a valid delimiter, tokenizing as delimiter", charRepr(c));
        return tokenizeDelimiter();
    }

    // 8. operators: try 2-char first, then 1-char
    if (current + 1 < source.size())
    {
        std::string two = source.substr(current, 2);
        if (operators.count(two))
        {
            DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "%s is a valid two-char operator, tokenizing as operator", two.c_str());
            return tokenizeOperator(two);
        }
    }
    {
        std::string one = source.substr(current, 1);
        if (operators.count(one))
        {
            DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "%s is a valid one-char operator, tokenizing as operator", one.c_str());
            return tokenizeOperator(one);
        }
    }

    // 9. unknown character
    advance();
    DEBUG_LOG(LogModule::Lexer, LogLevel::Debug, "Unknown character %s encountered, returning error token", charRepr(c));
    return Token(TokenType::Error, std::string("Unknown character: ") + c, startLine, startColumn);
}
