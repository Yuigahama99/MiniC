#include "include/lexer_tables.h"

#include <unordered_map>

namespace
{
const std::unordered_map<std::string, TokenType> kKeywords = {
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

const std::unordered_map<std::string, TokenType> kOperators = {
    {"+", TokenType::Plus},
    {"-", TokenType::Minus},
    {"*", TokenType::Multiply},
    {"/", TokenType::Divide},
    {"%", TokenType::Modulo},
    {"=", TokenType::Assign},
    {"==", TokenType::Equal},
    {"!=", TokenType::NotEqual},
    {"<", TokenType::Less},
    {"<=", TokenType::LessEqual},
    {">", TokenType::Greater},
    {">=", TokenType::GreaterEqual},
    {"&&", TokenType::And},
    {"||", TokenType::Or},
    {"!", TokenType::Not},
};

const std::unordered_map<char, TokenType> kDelimiters = {
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
}

std::optional<TokenType> keywordTokenType(const std::string &lexeme)
{
    auto it = kKeywords.find(lexeme);
    if (it == kKeywords.end())
    {
        return std::nullopt;
    }
    return it->second;
}

std::optional<TokenType> operatorTokenType(const std::string &lexeme)
{
    auto it = kOperators.find(lexeme);
    if (it == kOperators.end())
    {
        return std::nullopt;
    }
    return it->second;
}

std::optional<TokenType> delimiterTokenType(char c)
{
    auto it = kDelimiters.find(c);
    if (it == kDelimiters.end())
    {
        return std::nullopt;
    }
    return it->second;
}
