#include "parser/include/parser.h"
#include "common/debug.h"

#include <algorithm>
#include <string>

Parser::Parser(Lexer &lexer) : lexer(lexer) {}

//----------------------------------------
// Token cursor API
//----------------------------------------
/**
 * @brief Look at the current token in the token stream
 * 
 * @return [const Token&] the current token in the token stream
 */
const Token &Parser::peek()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    if (!currentToken.has_value())
    {
        currentToken = lexer.nextToken();
    }
    return *currentToken;
}

/**
 * @brief Peek the current token in the token stream and consume it
 * 
 * @return [Token] The current token in the token stream
 */
Token Parser::advance()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    Token token = peek();
    currentToken = lexer.nextToken();
    DEBUG_LOG(LogModule::Parser, LogLevel::Debug,
        "advance() consumed token at %d:%d: %s", token.line, token.column, token.lexeme.c_str());
    return token;
}

/**
 * @brief Check if the token type matches the expected type
 * 
 * @param type The expected token type
 * @return [true] Token type matches the expected type
 * @return [false] Token type does not match the expected type
 */
bool Parser::check(TokenType type)
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    return peek().type == type;
}

/**
 * @brief Check the token type. If match the expected type, consume it.
 * 
 * @param type The expected token type
 * @return [true] Token type matches the expected type
 * @return [false] Token type does not match the expected type
 */
bool Parser::match(TokenType type)
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    if (!check(type))
    {
        return false;
    }
    advance();
    return true;
}

/**
 * @brief Check if the current token is the expected type. If yes, consume it and return.
 * 
 * @param type expected token type 
 * @param message diagnostic message if mismatch encountered
 * @return [Token] The current token getting parsed or TokenType::Error if mismatch 
 */
Token Parser::expect(TokenType type, const std::string &message)
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    if (check(type))
    {
        return advance();
    }
    DEBUG_LOG(LogModule::Parser, LogLevel::Warn, "expect() mismatch: %s", message.c_str());
    addDiagnostic(DiagnosticSeverity::Error, peek(), message);
    return Token(TokenType::Error, "", peek().line, peek().column);
}

//----------------------------------------
// Diagnostic helpers
//----------------------------------------
/**
 * @brief Store diagnostic once warning/error detected. If it's an error, increase error count and turn
 *        on panic mode for synchronizing. If exceeding max error count, set fatalError flag to true.
 * 
 * @param severity diagnostic severity, warning or error
 * @param token The current token that triggers diagnostic
 * @param message The warning/error message about this diagnostic
 */
void Parser::addDiagnostic(DiagnosticSeverity severity, const Token &token, const std::string &message)
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    if (panicMode)
    {
        // If in panic mode, do not add new diagnostics. Recover first.
        return;
    }

    ParseDiagnostic d;
    d.severity = severity;
    d.span.start = {token.line, token.column};
    d.span.end = {token.line, token.column};
    d.message.assign(message);
    diagnostics.push_back(std::move(d));
    DEBUG_LOG(LogModule::Parser, LogLevel::Warn,
        "Diagnostic added at %d:%d: %s", token.line, token.column, message.c_str());

    if (severity == DiagnosticSeverity::Error)
    {
        errorCount++;
        panicMode = true;
        if (errorCount >= maxErrors)
        {
            fatalError = true;
        }
    }
}

/**
 * @brief Check if the type passed in is keyword
 * 
 * @param type The type need to be examed
 * @return [true] The examed type is keyword
 * @return [false] The examed type is not keyword
 */
bool Parser::isTypeKeyword(TokenType type) const
{
    return type == TokenType::KeywordInt ||
           type == TokenType::KeywordFloat ||
           type == TokenType::KeywordChar ||
           type == TokenType::KeywordString ||
           type == TokenType::KeywordVoid;
}

/**
 * @brief Check if the current token is the boundary of a statement
 *        Note, semicolon is the only possible terminator for semicolon-terminated statement
 *        RBrace is the terminator for block statement
 *        EOF is the special terminator for the whole program
 *        Others is the start symbol of a new statement
 * 
 * @return [true] The current token is the boundary of a statement
 * @return [false] The current token is not the boundary of a statement
 */
bool Parser::isAtStatementBoundary() const
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    TokenType type = currentToken.has_value() ? currentToken->type : TokenType::EndOfFile;
    return type == TokenType::Semicolon ||
           type == TokenType::RBrace ||
           type == TokenType::KeywordIf ||
           type == TokenType::KeywordWhile ||
           type == TokenType::KeywordFor ||
           type == TokenType::KeywordReturn ||
           type == TokenType::LBrace ||
           type == TokenType::EndOfFile ||
           isTypeKeyword(type);
}

/**
 * @brief Synchronize to the closest statement boundary when panic mode triggered by parse error
 *        Note, Semicolon is absolutely the end of a bad statement. So once we synchronize to that,
 *        we consume it and move forward to the next token. For other bundary, further processing 
 *        is required.
 * 
 */
void Parser::synchronizeStatementLevel()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    panicMode = false;

    while (!check(TokenType::EndOfFile))
    {
        // Consume the semicolon, since it's the end of semicolon-terminated statement
        if (peek().type == TokenType::Semicolon)
        {
            advance();
            return;
        }

        // Other boundary needs more processing, do not consume here
        if (isAtStatementBoundary())
        {
            return;
        }

        advance();
    }
}

/**
 * @brief Synchronize parser at top-level by skipping tokens until next type keyword or EOF.
 *
 */
void Parser::synchronizeTopLevel()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    panicMode = false;

    while (!check(TokenType::EndOfFile) && !isTypeKeyword(peek().type))
    {
        advance();
    }
}

/**
 * @brief Check if fatalError flag is set
 * 
 * @return [true] current program parsing has fatal error
 * @return [false] current program parsing does not have fatal error 
 */
bool Parser::hasFatalError() const
{
    return fatalError;
}

//----------------------------------------
// Parser Entrance
//----------------------------------------
/**
 * @brief Parser entrance. Start from top-level parsing: parseProgram()
 * 
 * @return [ParseResult] Parsing result for a progam. Containing diagnostics and status.
 *                       ParseStatus::Aborted - if pasing encountered a fatal error that can not recover
 *                       ParseStatus::RcoveredWithErrors - if no fatal errors but diagnostics is not empty
 *                       ParseStatus::Success - if parsing succeeded with no diagnostics
 *                       
 */
ParseResult Parser::parse()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    ParseResult result;
    result.program = parseProgram();
    result.diagnostics = std::move(diagnostics);

    if (fatalError)
    {
        result.status = ParseStatus::Aborted;
    }
    // Find out if diagnostics contain errors, warning treated as a pass
    else if (std::any_of(result.diagnostics.begin(), result.diagnostics.end(),
        [](const ParseDiagnostic &d) { return d.severity == DiagnosticSeverity::Error; }))
    {
        result.status = ParseStatus::RecoveredWithErrors;
    }
    else
    {
        result.status = ParseStatus::Success;
    }

    DEBUG_LOG(LogModule::Parser, LogLevel::Info,
        "Parse finished with status=%d, diagnostics=%zu", static_cast<int>(result.status), result.diagnostics.size());
    return result;
}
