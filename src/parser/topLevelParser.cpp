#include "parser/include/parser.h"
#include "common/debug.h"

/**
 * @brief Parsing entrance for program. This is the toppest level of MiniC parsing
 *        Program is made of function-definitions
 * 
 * @return [std::unique_ptr<Program>] Root of AST for this program
 */
std::unique_ptr<Program> Parser::parseProgram()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    auto program = std::make_unique<Program>();
    program->span.start = {peek().line, peek().column};

    while (!check(TokenType::EndOfFile) && !fatalError)
    {
        // In MiniC, program = function-definition*
        // function-definition = type identifier "(" parameter-list? ")" compound-statement
        // So if we detect type keyword, initiate parsing
        if (isTypeKeyword(peek().type))
        {
            auto func = parseFunctionDecl();
            if (func)
            {
                // Push functionDel node as child of Program root
                program->functions.push_back(std::move(func));
            }
        }
        else
        {
            DEBUG_LOG(LogModule::Parser, LogLevel::Warn,
                "Unexpected top-level token at %d:%d: %s", peek().line, peek().column, peek().lexeme.c_str());
            addDiagnostic(DiagnosticSeverity::Error, peek(),
                std::string("expected type specifier at top level, got '") + peek().lexeme + "'");
            synchronizeTopLevel();
        }
    }

    program->span.end = {peek().line, peek().column};
    DEBUG_LOG(LogModule::Parser, LogLevel::Info,
        "Program parse complete, function count: %zu", program->functions.size());
    return program;
}

/**
 * @brief Check if current token is type lexeme, consume it and return the type name or throw 
 *        error if not one of the defined type name.
 * 
 * @return [TypeName] The type name of current token, or TypeName::Error
 */
TypeName Parser::parseType()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    Token token = peek();
    switch (token.type)
    {
    case TokenType::KeywordInt:    advance(); return TypeName::Int;
    case TokenType::KeywordFloat:  advance(); return TypeName::Float;
    case TokenType::KeywordChar:   advance(); return TypeName::Char;
    case TokenType::KeywordString: advance(); return TypeName::String;
    case TokenType::KeywordVoid:   advance(); return TypeName::Void;
    default:
        addDiagnostic(DiagnosticSeverity::Error, token,
            std::string("expected type specifier, got '") + token.lexeme + "'");
        return TypeName::Error;
    }
}

/**
 * @brief Parsing entrance for function definition.
 * 
 * @return [std::unique_ptr<FunctionDecl>]  Node for the function, nullptr if parse error
 */
std::unique_ptr<FunctionDecl> Parser::parseFunctionDecl()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    auto func = std::make_unique<FunctionDecl>();
    func->span.start = {peek().line, peek().column};

    // function-definition = type identifier "(" parameter-list? ")" compound-statement
    // Parse type
    func->returnType = parseType();
    if (func->returnType == TypeName::Error)
    {
        synchronizeStatementLevel();
        return nullptr;
    }

    // Parse identifier (function name)
    Token nameToken = expect(TokenType::Identifier, "expected function name");
    if (nameToken.type == TokenType::Error)
    {
        synchronizeStatementLevel();
        return nullptr;
    }
    func->name = std::move(nameToken.lexeme);
    DEBUG_LOG(LogModule::Parser, LogLevel::Debug, "Parsing function declaration: %s", func->name.c_str());

    // Parse "("
    if (expect(TokenType::LParen, "expected '(' after function name").type == TokenType::Error)
    {
        synchronizeStatementLevel();
        return nullptr;
    }

    // Parse parameter-list?
    func->params = parseParamList();

    // Parse ")"
    if (expect(TokenType::RParen, "expected ')' after parameter list").type == TokenType::Error)
    {
        synchronizeStatementLevel();
        return nullptr;
    }

    // Parse compound-statement
    func->body = parseBlockStmt();
    if (!func->body)
    {
        return nullptr;
    }

    func->span.end = func->body->span.end;
    return func;
}

/**
 * @brief Parse the optional parameter list for function definition
 * 
 * @return [std::vector<ParamDecl>] empty if no params, or the parsed param list
 */
std::vector<ParamDecl> Parser::parseParamList()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    std::vector<ParamDecl> params;

    // Parameter-list is optional. If directly closed by RParen, return empty vector.
    if (check(TokenType::RParen))
    {
        return params;
    }

    do
    {
        ParamDecl param;
        param.span.start = {peek().line, peek().column};

        // parameter = type identifier
        // Parse type
        param.type = parseType();
        if (param.type == TypeName::Error)
        {
            break;
        }

        // Parse identifier
        Token nameToken = expect(TokenType::Identifier, "expected parameter name");
        if (nameToken.type == TokenType::Error)
        {
            break;
        }

        param.name = std::move(nameToken.lexeme);
        param.span.end = {nameToken.line, nameToken.column};
        params.push_back(std::move(param));
    } while (match(TokenType::Comma)); // expect will consume the lexeme.

    DEBUG_LOG(LogModule::Parser, LogLevel::Debug, "Parsed parameter count: %zu", params.size());
    return params;
}
