#include <iostream>
#include <fstream>
#include <utility>

#include "common/debug.h"
#include "lexer/include/lexer.h"

const char *tokenTypeName(TokenType type)
{
    switch (type)
    {
    case TokenType::KeywordInt:
        return "KeywordInt";
    case TokenType::KeywordFloat:
        return "KeywordFloat";
    case TokenType::KeywordChar:
        return "KeywordChar";
    case TokenType::KeywordVoid:
        return "KeywordVoid";
    case TokenType::KeywordIf:
        return "KeywordIf";
    case TokenType::KeywordElse:
        return "KeywordElse";
    case TokenType::KeywordWhile:
        return "KeywordWhile";
    case TokenType::KeywordFor:
        return "KeywordFor";
    case TokenType::KeywordReturn:
        return "KeywordReturn";
    case TokenType::Identifier:
        return "Identifier";
    case TokenType::IntLiteral:
        return "IntLiteral";
    case TokenType::FloatLiteral:
        return "FloatLiteral";
    case TokenType::CharLiteral:
        return "CharLiteral";
    case TokenType::StringLiteral:
        return "StringLiteral";
    case TokenType::Plus:
        return "Plus";
    case TokenType::Minus:
        return "Minus";
    case TokenType::Multiply:
        return "Multiply";
    case TokenType::Divide:
        return "Divide";
    case TokenType::Modulo:
        return "Modulo";
    case TokenType::Assign:
        return "Assign";
    case TokenType::Equal:
        return "Equal";
    case TokenType::NotEqual:
        return "NotEqual";
    case TokenType::Less:
        return "Less";
    case TokenType::LessEqual:
        return "LessEqual";
    case TokenType::Greater:
        return "Greater";
    case TokenType::GreaterEqual:
        return "GreaterEqual";
    case TokenType::And:
        return "And";
    case TokenType::Or:
        return "Or";
    case TokenType::Not:
        return "Not";
    case TokenType::LParen:
        return "LParen";
    case TokenType::RParen:
        return "RParen";
    case TokenType::LBrace:
        return "LBrace";
    case TokenType::RBrace:
        return "RBrace";
    case TokenType::LBracket:
        return "LBracket";
    case TokenType::RBracket:
        return "RBracket";
    case TokenType::Semicolon:
        return "Semicolon";
    case TokenType::Comma:
        return "Comma";
    case TokenType::Dot:
        return "Dot";
    case TokenType::EndOfFile:
        return "EndOfFile";
    case TokenType::Error:
        return "Error";
    default:
        return "Unknown";
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open())
    {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::string source{std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>()};
    inputFile.close();

    DEBUG_LOG(LogModule::Main, LogLevel::Info, "Loaded source file: %s (%zu bytes)", argv[1], source.size());

    Lexer lexer(std::move(source));

    while (true)
    {
        Token token = lexer.nextToken();
        if (token.type == TokenType::EndOfFile)
        {
            break;
        }

        DEBUG_LOG(LogModule::Main, LogLevel::Debug, "Token: %s (Type: %s, Line: %d, Col: %d)",
                  token.lexeme.c_str(), tokenTypeName(token.type), token.line, token.column);

        std::cout << "Token: " << token.lexeme
                  << " (Type: " << tokenTypeName(token.type)
                  << ", Line: " << token.line
                  << ", Column: " << token.column
                  << ")" << std::endl;
    }

    DEBUG_LOG(LogModule::Main, LogLevel::Info, "Lexing complete for: %s", argv[1]);
    return 0;
}
