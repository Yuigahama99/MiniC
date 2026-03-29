#pragma once

#include "ast.h"
#include "lexer/include/lexer.h"

#include <string>
#include <vector>

enum class DiagnosticSeverity
{
    Error,
    Warning
};

struct ParseDiagnostic
{
    DiagnosticSeverity severity = DiagnosticSeverity::Error;
    SourceSpan span{};
    std::string message;
};

enum class ParseStatus
{
    Success,
    RecoveredWithErrors,
    Aborted
};

struct ParseResult
{
    std::unique_ptr<Program> program;
    std::vector<ParseDiagnostic> diagnostics;
    ParseStatus status = ParseStatus::Success;
};

class Parser
{
public:
    explicit Parser(Lexer &lexer);

    ParseResult parse();
    bool hasFatalError() const;

private:
    Lexer &lexer;
    bool fatalError = false;
};