#include "parser/include/parser.h"
#include "common/debug.h"

/**
 * @brief Expression parser entrance.
 *        expression = assignment
 * 
 * @return [std::unique_ptr<Expr>] Pointer containing the parsed expression
 */
std::unique_ptr<Expr> Parser::parseExpression()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    return parseAssignment();
}

/**
 * @brief Parsing assignment expression.
 *        assignment = identifier "=" assignment | logical-or
 * 
 * @return [std::unique_ptr<Expr>] Pointer containing assignment expression or lower-precedence expression
 */
std::unique_ptr<Expr> Parser::parseAssignment()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    auto expr = parseLogicalOr();

    if (check(TokenType::Assign))
    {
        Token op = advance();
        DEBUG_LOG(LogModule::Parser, LogLevel::Debug,
            "Parsing assignment expression at %d:%d", op.line, op.column);
        auto value = parseAssignment(); // right-associative
        auto assign = std::make_unique<AssignExpr>();
        assign->span.start = expr->span.start;
        assign->target = std::move(expr);
        assign->value = std::move(value);
        assign->span.end = assign->value->span.end;
        return assign;
    }

    return expr;
}

/**
 * @brief Parsing logical-or expression.
 *        logical-or = logical-and ("||" logical-and)*
 * 
 * @return [std::unique_ptr<Expr>] Pointer containing the parsed logical-or expression
 */
std::unique_ptr<Expr> Parser::parseLogicalOr()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    auto left = parseLogicalAnd();

    while (check(TokenType::Or))
    {
        advance();
        auto right = parseLogicalAnd();
        auto binary = std::make_unique<BinaryExpr>();
        binary->op = BinaryOp::LogicalOr;
        binary->span.start = left->span.start;
        binary->left = std::move(left);
        binary->right = std::move(right);
        binary->span.end = binary->right->span.end;
        left = std::move(binary);
    }

    return left;
}

/**
 * @brief Parsing logical-and expression.
 *        logical-and = equality ("&&" equality)*
 * 
 * @return [std::unique_ptr<Expr>] Pointer containing the parsed logical-and expression
 */
std::unique_ptr<Expr> Parser::parseLogicalAnd()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    auto left = parseEquality();

    while (check(TokenType::And))
    {
        advance();
        auto right = parseEquality();
        auto binary = std::make_unique<BinaryExpr>();
        binary->op = BinaryOp::LogicalAnd;
        binary->span.start = left->span.start;
        binary->left = std::move(left);
        binary->right = std::move(right);
        binary->span.end = binary->right->span.end;
        left = std::move(binary);
    }

    return left;
}

/**
 * @brief Parsing equality expression.
 *        equality = relational (("==" | "!=") relational)*
 * 
 * @return [std::unique_ptr<Expr>] Pointer containing the parsed equality expression
 */
std::unique_ptr<Expr> Parser::parseEquality()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    auto left = parseRelational();

    while (check(TokenType::Equal) || check(TokenType::NotEqual))
    {
        Token op = advance();
        BinaryOp binOp = (op.type == TokenType::Equal) ? BinaryOp::Equal : BinaryOp::NotEqual;
        auto right = parseRelational();
        auto binary = std::make_unique<BinaryExpr>();
        binary->op = binOp;
        binary->span.start = left->span.start;
        binary->left = std::move(left);
        binary->right = std::move(right);
        binary->span.end = binary->right->span.end;
        left = std::move(binary);
    }

    return left;
}

/**
 * @brief Parsing relational expression.
 *        relational = additive (("<" | "<=" | ">" | ">=") additive)*
 * 
 * @return [std::unique_ptr<Expr>] Pointer containing the parsed relational expression
 */
std::unique_ptr<Expr> Parser::parseRelational()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    auto left = parseAdditive();

    while (check(TokenType::Less) || check(TokenType::LessEqual) ||
           check(TokenType::Greater) || check(TokenType::GreaterEqual))
    {
        Token op = advance();
        BinaryOp binOp;
        switch (op.type)
        {
        case TokenType::Less:         binOp = BinaryOp::Less; break;
        case TokenType::LessEqual:    binOp = BinaryOp::LessEqual; break;
        case TokenType::Greater:      binOp = BinaryOp::Greater; break;
        case TokenType::GreaterEqual: binOp = BinaryOp::GreaterEqual; break;
        default:                      binOp = BinaryOp::Less; break;
        }
        auto right = parseAdditive();
        auto binary = std::make_unique<BinaryExpr>();
        binary->op = binOp;
        binary->span.start = left->span.start;
        binary->left = std::move(left);
        binary->right = std::move(right);
        binary->span.end = binary->right->span.end;
        left = std::move(binary);
    }

    return left;
}

/**
 * @brief Parsing additive expression.
 *        additive = multiplicative (("+" | "-") multiplicative)*
 * 
 * @return [std::unique_ptr<Expr>] Pointer containing the parsed additive expression
 */
std::unique_ptr<Expr> Parser::parseAdditive()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    auto left = parseMultiplicative();

    while (check(TokenType::Plus) || check(TokenType::Minus))
    {
        Token op = advance();
        BinaryOp binOp = (op.type == TokenType::Plus) ? BinaryOp::Add : BinaryOp::Subtract;
        auto right = parseMultiplicative();
        auto binary = std::make_unique<BinaryExpr>();
        binary->op = binOp;
        binary->span.start = left->span.start;
        binary->left = std::move(left);
        binary->right = std::move(right);
        binary->span.end = binary->right->span.end;
        left = std::move(binary);
    }

    return left;
}

/**
 * @brief Parsing multiplicative expression.
 *        multiplicative = unary (("*" | "/" | "%") unary)*
 * 
 * @return [std::unique_ptr<Expr>] Pointer containing the parsed multiplicative expression
 */
std::unique_ptr<Expr> Parser::parseMultiplicative()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    auto left = parseUnary();

    while (check(TokenType::Multiply) || check(TokenType::Divide) || check(TokenType::Modulo))
    {
        Token op = advance();
        BinaryOp binOp;
        switch (op.type)
        {
        case TokenType::Multiply: binOp = BinaryOp::Multiply; break;
        case TokenType::Divide:   binOp = BinaryOp::Divide; break;
        case TokenType::Modulo:   binOp = BinaryOp::Modulo; break;
        default:                  binOp = BinaryOp::Multiply; break;
        }
        auto right = parseUnary();
        auto binary = std::make_unique<BinaryExpr>();
        binary->op = binOp;
        binary->span.start = left->span.start;
        binary->left = std::move(left);
        binary->right = std::move(right);
        binary->span.end = binary->right->span.end;
        left = std::move(binary);
    }

    return left;
}

/**
 * @brief Parsing unary expression.
 *        unary = ("!" | "-" | "+") unary | postfix
 * 
 * @return [std::unique_ptr<Expr>] Pointer containing the parsed unary/postfix expression
 */
std::unique_ptr<Expr> Parser::parseUnary()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    if (check(TokenType::Not) || check(TokenType::Minus) || check(TokenType::Plus))
    {
        Token op = advance();
        UnaryOp unOp;
        switch (op.type)
        {
        case TokenType::Not:   unOp = UnaryOp::Not; break;
        case TokenType::Minus: unOp = UnaryOp::Minus; break;
        case TokenType::Plus:  unOp = UnaryOp::Plus; break;
        default:               unOp = UnaryOp::Plus; break;
        }
        auto operand = parseUnary();
        auto expr = std::make_unique<UnaryExpr>();
        expr->op = unOp;
        expr->span.start = {op.line, op.column};
        expr->operand = std::move(operand);
        expr->span.end = expr->operand->span.end;
        return expr;
    }

    return parsePostfix();
}

/**
 * @brief Parsing postfix expression.
 *        postfix = primary ("(" argument-list? ")")*
 * 
 * @return [std::unique_ptr<Expr>] Pointer containing identifier/literal/grouping/call expression
 */
std::unique_ptr<Expr> Parser::parsePostfix()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    auto expr = parsePrimary();

    while (check(TokenType::LParen))
    {
        advance(); // consume '('
        auto args = parseArgumentList();
        Token rparen = expect(TokenType::RParen, "expected ')' after arguments");

        auto call = std::make_unique<CallExpr>();
        call->span.start = expr->span.start;
        call->callee = std::move(expr);
        call->args = std::move(args);
        call->span.end = {rparen.line, rparen.column};
        expr = std::move(call);
    }

    return expr;
}

/**
 * @brief Parsing argument list used by function call expression.
 *        argument-list = expression ("," expression)*
 * 
 * @return [std::vector<std::unique_ptr<Expr>>] Parsed argument expressions, empty if no arguments
 */
std::vector<std::unique_ptr<Expr>> Parser::parseArgumentList()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    std::vector<std::unique_ptr<Expr>> args;

    if (check(TokenType::RParen))
    {
        return args;
    }

    args.push_back(parseExpression());

    while (match(TokenType::Comma))
    {
        args.push_back(parseExpression());
    }

    return args;
}

/**
 * @brief Parsing primary expression.
 *        primary = identifier | literal | "(" expression ")"
 * 
 * @return [std::unique_ptr<Expr>] Parsed primary expression node or an ErrorExpr for recovery
 */
std::unique_ptr<Expr> Parser::parsePrimary()
{
    TRACE_ENTER_EXIT(LogModule::Parser);

    Token token = peek();

    // identifier
    if (check(TokenType::Identifier))
    {
        DEBUG_LOG(LogModule::Parser, LogLevel::Debug, "Identifier primary: %s", token.lexeme.c_str());
        advance();
        auto expr = std::make_unique<IdentifierExpr>();
        expr->lexeme = std::move(token.lexeme);
        expr->span.start = {token.line, token.column};
        expr->span.end = {token.line, token.column + static_cast<int>(token.lexeme.size()) - 1};
        return expr;
    }

    // literals
    if (check(TokenType::IntLiteral) || check(TokenType::FloatLiteral) ||
        check(TokenType::CharLiteral) || check(TokenType::StringLiteral))
    {
        DEBUG_LOG(LogModule::Parser, LogLevel::Debug, "Literal primary: %s", token.lexeme.c_str());
        advance();
        auto expr = std::make_unique<LiteralExpr>();
        expr->lexeme = std::move(token.lexeme);
        expr->span.start = {token.line, token.column};
        expr->span.end = {token.line, token.column + static_cast<int>(token.lexeme.size()) - 1};
        return expr;
    }

    // grouping: "(" expression ")"
    if (check(TokenType::LParen))
    {
        Token lparen = advance();
        auto inner = parseExpression();
        Token rparen = expect(TokenType::RParen, "expected ')' after expression");
        auto expr = std::make_unique<GroupingExpr>();
        expr->inner = std::move(inner);
        expr->span.start = {lparen.line, lparen.column};
        expr->span.end = {rparen.line, rparen.column};
        return expr;
    }

    // error recovery: unexpected token
    addDiagnostic(DiagnosticSeverity::Error, token,
        std::string("expected expression, got '") + token.lexeme + "'");
    DEBUG_LOG(LogModule::Parser, LogLevel::Warn,
        "Invalid primary token at %d:%d: %s", token.line, token.column, token.lexeme.c_str());
    advance();
    auto errExpr = std::make_unique<ErrorExpr>();
    errExpr->message = std::string("unexpected token: ") + token.lexeme;
    errExpr->span.start = {token.line, token.column};
    errExpr->span.end = {token.line, token.column};
    return errExpr;
}
