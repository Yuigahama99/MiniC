#include "parser/include/parser.h"

Parser::Parser(Lexer &lexer) : lexer(lexer) {}

ParseResult Parser::parse()
{
    ParseResult result;
    result. program = std::make_unique<Program>();

    while (true)
    {
        Token token = lexer.nextToken();

        if (token.type == TokenType::Error)
        {
            ParseDiagnostic d;
            d.severity = DiagnosticSeverity::Error;
            d.span.start = {token.line, token.column};
            d.span.end = {token.line, token.column};
            d.message = "lexical error token: " + token.lexeme;
            result.diagnostics.push_back(std::move(d));
        }

        if (token.type == TokenType::EndOfFile)
        {
            break;
        }
    }

    if (!result.diagnostics.empty())
    {
        result.status = ParseStatus::RecoveredWithErrors;
    }
    else 
    {
        result.status = ParseStatus::Success;
    }

    return result;
}

bool Parser::hasFatalError() const
{
    return fatalError;
}