#pragma once

#include <memory>
#include <string>
#include <vector>

//----------------------------------------
// Source location model
//----------------------------------------
struct SourceLocation
{
    int line = 1;
    int column = 1;
};

struct SourceSpan
{
    SourceLocation start{};
    SourceLocation end{};
};

//----------------------------------------
// Common enums
//----------------------------------------
enum class TypeName
{
    Int,
    Float,
    Char,
    String,
    Void,
    Error
};

enum class BinaryOp
{
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    LogicalAnd,
    LogicalOr
};

enum class UnaryOp
{
    Plus,
    Minus,
    Not
};

//----------------------------------------
// Base nodes
//----------------------------------------
struct Node
{
    SourceSpan span{};
    virtual ~Node() = default;
};

struct Stmt : Node
{
    virtual ~Stmt() = default;
};

struct Expr : Node
{
    virtual ~Expr() = default;
};

//----------------------------------------
// Top-level declarations
//----------------------------------------
// Grammar: parameter = type identifier
struct ParamDecl
{
    TypeName type = TypeName::Error;
    std::string name;
    SourceSpan span{};
};

struct BlockStmt;

// Grammar: function-definition = type identifier "(" parameter-list? ")" compound-statement
struct FunctionDecl : Node
{
    TypeName returnType = TypeName::Error;
    std::string name;
    std::vector<ParamDecl> params;
    std::unique_ptr<BlockStmt> body;
};

// Grammar: program = function-definition*
struct Program : Node
{
    std::vector<std::unique_ptr<FunctionDecl>> functions;
};

//----------------------------------------
// Statements
//----------------------------------------
// Grammar: n/a (recovery placeholder for invalid statement)
struct ErrorStmt : Stmt
{
    std::string message;
};

// Grammar: compound-statement = "{" statement* "}"
struct BlockStmt : Stmt
{
    std::vector<std::unique_ptr<Stmt>> statements;
};

// Grammar: declaration = type identifier ("=" expression)? ";"
struct DeclStmt : Stmt
{
    TypeName type = TypeName::Error;
    std::string name;
    std::unique_ptr<Expr> initializer; // optional
};

// Grammar: expression-statement = expression? ";"
struct ExprStmt : Stmt
{
    std::unique_ptr<Expr> expr; // optional
};

// Grammar: if-statement = "if" "(" expression ")" statement ("else" statement)?
struct IfStmt : Stmt
{
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch; // optional
};

// Grammar: while-statement = "while" "(" expression ")" statement
struct WhileStmt : Stmt
{
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
};

// Grammar: for-statement = "for" "(" for-init ";" for-condition? ";" for-update? ")" statement
struct ForStmt : Stmt
{
    std::unique_ptr<Stmt> init; // optional
    std::unique_ptr<Expr> condition; // optional
    std::unique_ptr<Expr> update; // optional
    std::unique_ptr<Stmt> body;
};

// Grammar: return-statement = "return" expression? ";"
struct ReturnStmt : Stmt
{
    std::unique_ptr<Expr> value; // optional
};

//----------------------------------------
// Expressions
//----------------------------------------
// Grammar: n/a (recovery placeholder for invalid expression)
struct ErrorExpr : Expr
{
    std::string message;
};

// Grammar: assignment = identifier "=" assignment | logical-or
struct AssignExpr : Expr
{
    std::unique_ptr<Expr> target;
    std::unique_ptr<Expr> value;
};

// Grammar: logical-or / logical-and / equality / relational / additive / multiplicative
struct BinaryExpr : Expr
{
    BinaryOp op = BinaryOp::Add;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

// Grammar: unary = ("!" | "-" | "+") unary | postfix
struct UnaryExpr : Expr
{
    UnaryOp op = UnaryOp::Plus;
    std::unique_ptr<Expr> operand;
};

// Grammar: postfix = primary ("(" argument-list? ")")*
struct CallExpr : Expr
{
    std::unique_ptr<Expr> callee;
    std::vector<std::unique_ptr<Expr>> args;
};

// Grammar: primary = identifier
struct IdentifierExpr : Expr
{
    std::string lexeme;
};

// Grammar: primary = literal
struct LiteralExpr : Expr
{
    std::string lexeme;
};

// Grammar: primary = "(" expression ")"
struct GroupingExpr : Expr
{
    std::unique_ptr<Expr> inner;
};