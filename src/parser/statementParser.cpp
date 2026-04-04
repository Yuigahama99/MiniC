#include "parser/include/parser.h"
#include "common/debug.h"

/**
 * @brief Statement parser dispatcher.
 *        Peek at the current token to dispatch to corresponding statement parser.
 * 
 * @return [std::unique_ptr<Stmt>] pointer containing the statement get parsed, or an error statement
 */
std::unique_ptr<Stmt> Parser::parseStatement()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    // If fatal error detected, do not proceed to parse
    if (fatalError)
    {
        DEBUG_LOG(LogModule::Parser, LogLevel::Warn, "parseStatement() skipped due to fatalError");
        return nullptr;
    }

    // handle lexer error tokens
    if (check(TokenType::Error))
    {
        Token errToken = advance();
        DEBUG_LOG(LogModule::Parser, LogLevel::Error,
            "Lexer error token at %d:%d: %s", errToken.line, errToken.column, errToken.lexeme.c_str());
        addDiagnostic(DiagnosticSeverity::Error, errToken,
            std::string("lexical error: ") + errToken.lexeme);
        synchronizeStatementLevel();
        auto errStmt = std::make_unique<ErrorStmt>();
        errStmt->message = std::move(errToken.lexeme);
        return errStmt;
    }

    // Peek at current token to dispatch
    DEBUG_LOG(LogModule::Parser, LogLevel::Debug,
        "Dispatch statement parser at %d:%d token='%s'", peek().line, peek().column, peek().lexeme.c_str());
    switch (peek().type)
    {
    case TokenType::LBrace:
        return parseBlockStmt(); // BlockStmt starts with '{'
    case TokenType::KeywordIf:
        return parseIfStmt(); // IfStmt starts with "if"
    case TokenType::KeywordWhile:
        return parseWhileStmt(); // WhileStmt starts with "while"
    case TokenType::KeywordFor:
        return parseForStmt(); // ForStmt starts with "for"
    case TokenType::KeywordReturn:
        return parseReturnStmt(); // ReturnStmt starts with "return"
    default:
        if (isTypeKeyword(peek().type))
        {
            return parseDeclStmt(); // DeclStmt starts with type keyword
        }
        return parseExprStmt(); // Any other case full into expression parsing
    }
}

/**
 * @brief Block statement parser, called by top level function parser.
 *        compound-statement = "{" statement* "}"
 * 
 * @return [std::unique_ptr<BlockStmt>] Pointer containing the block statement get parsed
 */
std::unique_ptr<BlockStmt> Parser::parseBlockStmt()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    auto block = std::make_unique<BlockStmt>();

    // Block statement starts with '{'
    Token lbrace = expect(TokenType::LBrace, "expected '{'");
    if (lbrace.type == TokenType::Error)
    {
        synchronizeStatementLevel();
        return block;
    }
    block->span.start = {lbrace.line, lbrace.column};

    while (!check(TokenType::RBrace) && !check(TokenType::EndOfFile) && !fatalError)
    {
        auto stmt = parseStatement();
        if (stmt)
        {
            block->statements.push_back(std::move(stmt));
        }
    }

    // Block statement ends with '}'
    Token rbrace = expect(TokenType::RBrace, "expected '}'");
    block->span.end = {rbrace.line, rbrace.column};
    DEBUG_LOG(LogModule::Parser, LogLevel::Debug,
        "Parsed block with %zu statements", block->statements.size());
    return block;
}

/**
 * @brief Parsing declaration statement.
 *        declaration = type identifier (= expression)?;
 * 
 * @return [std::unique_ptr<Stmt>] Pointer containing the declaration statement get parsed or an error statement
 */
std::unique_ptr<Stmt> Parser::parseDeclStmt()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    auto stmt = std::make_unique<DeclStmt>();
    stmt->span.start = {peek().line, peek().column};

    // DeclStmt starts with type keyword
    stmt->type = parseType();
    if (stmt->type == TypeName::Error)
    {
        synchronizeStatementLevel();
        // Diagnostic already added in parseType() if reached here
        return std::make_unique<ErrorStmt>();
    }

    // Parse the identifier
    Token nameToken = expect(TokenType::Identifier, "expected variable name");
    if (nameToken.type == TokenType::Error)
    {
        synchronizeStatementLevel();
        // Diagnostic already added in expect() if reached here
        return std::make_unique<ErrorStmt>();
    }
    stmt->name = std::move(nameToken.lexeme);
    DEBUG_LOG(LogModule::Parser, LogLevel::Debug, "Parsing declaration for variable: %s", stmt->name.c_str());

    // If followed by '=' operator, parsing initializer
    if (match(TokenType::Assign))
    {
        // Initializer is an expression, dispatch to expressionParser
        stmt->initializer = parseExpression();
    }

    // DeclStmt must be followed by a semicolon to terminate
    Token semi = expect(TokenType::Semicolon, "expected ';' after declaration");
    stmt->span.end = {semi.line, semi.column};
    if (semi.type == TokenType::Error)
    {
        synchronizeStatementLevel();
    }
    return stmt;
}

/**
 * @brief Parsing expression statement
 *        expression-statement = expression?;
 * 
 * @return [std::unique_ptr<Stmt>] Pointer containing the expression statement get parsed
 */
std::unique_ptr<Stmt> Parser::parseExprStmt()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    auto stmt = std::make_unique<ExprStmt>();
    stmt->span.start = {peek().line, peek().column};

    // Since expression is optional, validate semicolon first
    if (check(TokenType::Semicolon))
    {
        Token semi = advance();
        stmt->span.end = {semi.line, semi.column};
        return stmt;
    }

    // Parsing expression, dispatch to expression parser
    stmt->expr = parseExpression();

    // Expression statement much be terminated by semicolon
    Token semi = expect(TokenType::Semicolon, "expected ';' after expression");
    stmt->span.end = {semi.line, semi.column};
    if (semi.type == TokenType::Error)
    {
        synchronizeStatementLevel();
    }
    return stmt;
}

/**
 * @brief Parsing if statement
 *        if-statement = if (expression) statement (else statement)?
 * 
 * @return [std::unique_ptr<Stmt>] Pointer containing the if statement get parsed or an error statement
 */
std::unique_ptr<Stmt> Parser::parseIfStmt()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    auto stmt = std::make_unique<IfStmt>();
    Token ifToken = advance(); // consume 'if'
    stmt->span.start = {ifToken.line, ifToken.column};

    // Parsing '('
    if (expect(TokenType::LParen, "expected '(' after 'if'").type == TokenType::Error)
    {
        synchronizeStatementLevel();
        return std::make_unique<ErrorStmt>();
    }

    // Parsing expression
    stmt->condition = parseExpression();

    // Parsing ')'
    if (expect(TokenType::RParen, "expected ')' after if condition").type == TokenType::Error)
    {
        synchronizeStatementLevel();
        return std::make_unique<ErrorStmt>();
    }

    // Parsing then branch statement
    stmt->thenBranch = parseStatement();

    // Check if this if statement has optional else block
    if (match(TokenType::KeywordElse))
    {
        // Parsing else branch statement
        DEBUG_LOG(LogModule::Parser, LogLevel::Debug, "else branch detected");
        stmt->elseBranch = parseStatement();
    }

    // Check if else branch exist, otherwise set ends with then branch
    if (stmt->elseBranch)
    {
        stmt->span.end = stmt->elseBranch->span.end;
    }
    else if (stmt->thenBranch)
    {
        stmt->span.end = stmt->thenBranch->span.end;
    }
    return stmt;
}

/**
 * @brief Parsing while statement
 *        while-statement = while (expression) statement
 * 
 * @return [std::unique_ptr<Stmt>] Pointer to the while statement get parsed or an error statement 
 */
std::unique_ptr<Stmt> Parser::parseWhileStmt()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    auto stmt = std::make_unique<WhileStmt>();
    Token whileToken = advance(); // consume 'while'
    stmt->span.start = {whileToken.line, whileToken.column};

    // Parsing '('
    if (expect(TokenType::LParen, "expected '(' after 'while'").type == TokenType::Error)
    {
        synchronizeStatementLevel();
        return std::make_unique<ErrorStmt>();
    }

    // Parsing condition expression
    stmt->condition = parseExpression();

    // Parsing ')'
    if (expect(TokenType::RParen, "expected ')' after while condition").type == TokenType::Error)
    {
        synchronizeStatementLevel();
        return std::make_unique<ErrorStmt>();
    }

    // Parsing the body statement
    stmt->body = parseStatement();
    if (stmt->body)
    {
        stmt->span.end = stmt->body->span.end;
    }
    return stmt;
}

/**
 * @brief Parsing for statement
 *        for-statement = for (for-init?; for-condition?; for-update?) statement
 * 
 * @return [std::unique_ptr<Stmt>] Pointer containing for statement get parsed
 */
std::unique_ptr<Stmt> Parser::parseForStmt()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    auto stmt = std::make_unique<ForStmt>();
    Token forToken = advance(); // consume 'for'
    stmt->span.start = {forToken.line, forToken.column};

    // Parsing '('
    if (expect(TokenType::LParen, "expected '(' after 'for'").type == TokenType::Error)
    {
        synchronizeStatementLevel();
        return std::make_unique<ErrorStmt>();
    }

    // Parsing for-init
    // for-init? = declaration-no-semi | expression
    if (!check(TokenType::Semicolon))
    {
        if (isTypeKeyword(peek().type))
        {
            DEBUG_LOG(LogModule::Parser, LogLevel::Debug, "for-init parsed as declaration");
            // for-init = declaration-no-semi
            // declaration-no-semi = type identifier ("=" expression)?
            // Handled it manually without calling parsingDeclStmt
            // Since we don't want to consume the ';' which belongs to for statement
            auto declStmt = std::make_unique<DeclStmt>();
            declStmt->span.start = {peek().line, peek().column};
            declStmt->type = parseType();

            Token nameToken = expect(TokenType::Identifier, "expected variable name in for-init");
            if (nameToken.type != TokenType::Error)
            {
                declStmt->name = std::move(nameToken.lexeme);
                if (match(TokenType::Assign))
                {
                    declStmt->initializer = parseExpression();
                }
            }
            declStmt->span.end = {peek().line, peek().column};
            stmt->init = std::move(declStmt);
        }
        else
        {   
            DEBUG_LOG(LogModule::Parser, LogLevel::Debug, "for-init parsed as expression");
            // for-init = expression
            auto initExpr = std::make_unique<ExprStmt>();
            initExpr->span.start = {peek().line, peek().column};

            // Dispatch to the expression parser
            initExpr->expr = parseExpression();

            initExpr->span.end = {peek().line, peek().column};
            stmt->init = std::move(initExpr);
        }
    }

    // Parsing the first ';'
    if (expect(TokenType::Semicolon, "expected ';' in for statement").type == TokenType::Error)
    {
        synchronizeStatementLevel();
        return std::make_unique<ErrorStmt>();
    }

    // Parsing for-condition
    // for-condition = expression
    if (!check(TokenType::Semicolon))
    {
        stmt->condition = parseExpression();
    }

    // Parsing the second ';'
    if (expect(TokenType::Semicolon, "expected ';' in for statement").type == TokenType::Error)
    {
        synchronizeStatementLevel();
        return std::make_unique<ErrorStmt>();
    }

    // Parsing for-update
    // for-update = expression
    if (!check(TokenType::RParen))
    {
        stmt->update = parseExpression();
    }

    // Parsing ')'
    if (expect(TokenType::RParen, "expected ')' after for clauses").type == TokenType::Error)
    {
        synchronizeStatementLevel();
        return std::make_unique<ErrorStmt>();
    }

    // Parsing body statement
    stmt->body = parseStatement();
    if (stmt->body)
    {
        stmt->span.end = stmt->body->span.end;
    }
    return stmt;
}

/**
 * @brief Parsing return statement
 *        return-statement = "return" expression? ";"
 * 
 * @return [std::unique_ptr<Stmt>] Pointer containing the return statement get parsed 
 */
std::unique_ptr<Stmt> Parser::parseReturnStmt()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    auto stmt = std::make_unique<ReturnStmt>();
    Token retToken = advance(); // consume 'return'
    stmt->span.start = {retToken.line, retToken.column};

    // Check if optional expression exist
    if (!check(TokenType::Semicolon))
    {   
        // Dispatch to the expression parser
        stmt->value = parseExpression();
    }

    // Parsing ';'
    Token semi = expect(TokenType::Semicolon, "expected ';' after return statement");
    
    stmt->span.end = {semi.line, semi.column};
    if (semi.type == TokenType::Error)
    {
        synchronizeStatementLevel();
    }
    return stmt;
}