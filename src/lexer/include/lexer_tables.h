#pragma once

#include "lexer/include/token.h"

#include <string>
#include <optional>

std::optional<TokenType> keywordTokenType(const std::string &lexeme);
std::optional<TokenType> operatorTokenType(const std::string &lexeme);
std::optional<TokenType> delimiterTokenType(char c);
