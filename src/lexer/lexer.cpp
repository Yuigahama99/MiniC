#include "lexer.h"


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
    if (current >= source.size())
        return '\0';
    return source[current];
}

/**
 * @brief Peeks at the next character in the source without advancing
 *
 * @return [char] the next character, or '\0' if at the end of the source
 */
char Lexer::peekNext() const
{
    if (current + 1 >= source.size())
        return '\0';
    return source[current + 1];
}

/**
 * @brief Advances the current position in the source and returns the character that was advanced over
 *
 * @return [char] the character that was advanced over, or '\0' if at the end of the source
 */
char Lexer::advance()
{
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

    current++;
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
    while (true)
    {
        char c = peek();

        switch (c)
        {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            advance();
            break;
        case '/':
            // check for comments
            if (peekNext() == '/')
            {
                // line comment, consume '//'
                advance();
                advance();

                // skip until end of line or end of file
                while(peek() != '\n' && peek() != '\0')
                {
                    advance();
                }
            }
            else if (peekNext() == '*')
            {
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
                    advance();
                    advance();
                }
                else
                {
                    // Unterminated block comment
                    return Token(TokenType::Error, "Unterminated block comment", startLine, startColumn);
                }
            }
            else
            {
                return std::nullopt; // not a comment, stop skipping
            }
            break;

        default:
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
        return Token(keywords.at(lexeme), lexeme, startLine, startColumn);
    }

    // otherwise, it's an identifier
    return Token(TokenType::Identifier, lexeme, startLine, startColumn);
}

/**
 * @brief Tokenizes a number (integer or float) from the source
 *
 * @return [Token] a token representing either an integer literal or a float literal, depending on the lexeme
 */
Token Lexer::tokenizeNumber()
{
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
        return Token(TokenType::FloatLiteral, lexeme, startLine, startColumn);
    }

    return Token(TokenType::IntLiteral, lexeme, startLine, startColumn);
}

/**
 * @brief Tokenizes a character literal from the source
 *
 * @return [Token] a token representing a character literal
 */
Token Lexer::tokenizeChar()
{
    int startLine = line;
    int startColumn = column;
    size_t start = current;

    // consume opening quote
    advance();

    char c = peek();
    if (c == '\0' || c == '\n')
    {
        // Unterminated char literal
        return Token(TokenType::Error, "Unterminated char literal", startLine, startColumn);
    }

    // escapes currently not supported for MiniC, reject
    if (c == '\\')
    {
        return Token(TokenType::Error, "Escape sequences not supported in char literals", startLine, startColumn);
    }

    // consume the character
    advance();

    // consume closing quote
    if (peek() != '\'')
    {
        // Unterminated char literal
        return Token(TokenType::Error, "Unterminated char literal", startLine, startColumn);
    }

    // consume closing quote
    advance();

    // extract lexeme
    std::string lexeme = source.substr(start, current - start);

    return Token(TokenType::CharLiteral, lexeme, startLine, startColumn);
}

/**
 * @brief Tokenizes a string literal from the source
 *
 * @return [Token] a token representing a string literal
 */
Token Lexer::tokenizeString()
{
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
            return Token(TokenType::Error, "Unterminated string literal", startLine, startColumn);
        }

        if (c == '"')
        {
            // consume closing double quote
            advance();
            break; // end of string literal
        }

        // escapes currently not supported for MiniC, reject
        if (c == '\\')
        {
            return Token(TokenType::Error, "Escape sequences not supported in string literals", startLine, startColumn);
        }

        // consume character
        advance();
    }

    // extract lexeme
    std::string lexeme = source.substr(start, current - start);

    return Token(TokenType::StringLiteral, lexeme, startLine, startColumn);
}

/**
 * @brief Tokenizes an operator from the source
 *
 * @return [Token] a token representing an operator
 */
Token Lexer::tokenizeOperator()
{
    int startLine = line;
    int startColumn = column;
    size_t start = current;

    // check 2-character operators first
    if(current + 1 < source.size())
    {
        std::string lexeme = source.substr(start, 2);
        if(operators.count(lexeme))
        {
            advance();
            advance();

            return Token(operators.at(lexeme), lexeme, startLine, startColumn);
        }
    }

    // check 1-character operators
    std::string lexeme = source.substr(start, 1);
    if(operators.count(lexeme))
    {
        advance();
        return Token(operators.at(lexeme), lexeme, startLine, startColumn);
    }

    // Unknown operator
    return Token(TokenType::Error, "Unknown operator: " + lexeme, startLine, startColumn);
}

/**
 * @brief Tokenizes a delimiter from the source
 *
 * @return [Token] a token representing a delimiter
 */
Token Lexer::tokenizeDelimiter()
{
    int startLine = line;
    int startColumn = column;
    size_t start = current;

    char c = peek();
    if (delimiters.count(c))
    {
        advance();
        std::string lexeme = source.substr(start, 1);
        return Token(delimiters.at(c), lexeme, startLine, startColumn);
    }

    // Unknown delimiter
    return Token(TokenType::Error, std::string("Unknown delimiter: ") + c, startLine, startColumn);
}

/**
 * @brief Get the next token from the source
 *
 * @return [Token] the next token, or an error token if an invalid token is encountered
 */
Token Lexer::nextToken()
{
    auto err = skipWhitespaceOrComment();
    if (err.has_value())
    {
        return *err;
    }

    char c = peek();
    if (c == '\0')
    {
        return Token(TokenType::EndOfFile, std::string(""), line, column);
    }

    if (isAlpha(c) || c == '_')
    {
        return tokenizeIdentifier();
    }

    if (isDigit(c))
    {
        return tokenizeNumber();
    }

    if (c == '\'')
    {
        return tokenizeChar();
    }

    if (c == '"')
    {
        return tokenizeString();
    }

    if (delimiters.count(c))
    {
        return tokenizeDelimiter();
    }

    return tokenizeOperator();
}
