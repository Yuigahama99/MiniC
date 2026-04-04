#pragma once

#include "ast.h"
#include "lexer/include/lexer.h"

#include <string>
#include <vector>
#include <optional>

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
    std::optional<Token> currentToken;
    std::vector<ParseDiagnostic> diagnostics;
    int errorCount = 0;
    bool panicMode = false;
    bool fatalError = false;

    static constexpr int maxErrors = 50;

    // token cursor API
    const Token &peek();
    Token advance();
    bool check(TokenType type);
    bool match(TokenType type);
    Token expect(TokenType type, const std::string &message);

    // diagnostic helpers
    void addDiagnostic(DiagnosticSeverity severity, const Token &token, const std::string &message);
    void synchronizeStatementLevel();
    void synchronizeTopLevel();
    bool isAtStatementBoundary() const;
    bool isTypeKeyword(TokenType type) const;

    // top-level parsing
    std::unique_ptr<Program> parseProgram();
    std::unique_ptr<FunctionDecl> parseFunctionDecl();
    std::vector<ParamDecl> parseParamList();
    TypeName parseType();

    // statement parsing
    std::unique_ptr<Stmt> parseStatement();
    std::unique_ptr<BlockStmt> parseBlockStmt();
    std::unique_ptr<Stmt> parseDeclStmt();
    std::unique_ptr<Stmt> parseExprStmt();
    std::unique_ptr<Stmt> parseIfStmt();
    std::unique_ptr<Stmt> parseWhileStmt();
    std::unique_ptr<Stmt> parseForStmt();
    std::unique_ptr<Stmt> parseReturnStmt();

    // expression parsing (precedence chain)
    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parseAssignment();
    std::unique_ptr<Expr> parseLogicalOr();
    std::unique_ptr<Expr> parseLogicalAnd();
    std::unique_ptr<Expr> parseEquality();
    std::unique_ptr<Expr> parseRelational();
    std::unique_ptr<Expr> parseAdditive();
    std::unique_ptr<Expr> parseMultiplicative();
    std::unique_ptr<Expr> parseUnary();
    std::unique_ptr<Expr> parsePostfix();
    std::unique_ptr<Expr> parsePrimary();
    std::vector<std::unique_ptr<Expr>> parseArgumentList();
};