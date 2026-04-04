#include <cstring>
#include <iostream>
#include <fstream>
#include <utility>

#include "common/debug.h"
#include "lexer/include/lexer.h"
#include "parser/include/parser.h"

enum class Stage { Lex, Parse };

int main(int argc, char *argv[])
{
    Stage stage = Stage::Parse;
    const char *inputPath = nullptr;

    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "--stage") == 0 && i + 1 < argc)
        {
            ++i;
            if (std::strcmp(argv[i], "lex") == 0)
                stage = Stage::Lex;
            else if (std::strcmp(argv[i], "parse") == 0)
                stage = Stage::Parse;
            else
            {
                std::cerr << "Unknown stage: " << argv[i] << std::endl;
                return 1;
            }
        }
        else if (argv[i][0] != '-')
        {
            inputPath = argv[i];
        }
        else
        {
            std::cerr << "Unknown option: " << argv[i] << std::endl;
            return 1;
        }
    }

    if (!inputPath)
    {
        std::cerr << "Usage: " << argv[0] << " [--stage lex|parse] <input_file>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(inputPath);
    if (!inputFile.is_open())
    {
        std::cerr << "Error: Could not open file " << inputPath << std::endl;
        return 1;
    }

    std::string source;
    source.assign(std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>());
    inputFile.close();

    DEBUG_LOG(LogModule::Main, LogLevel::Info, "Loaded source file: %s (%zu bytes)", inputPath, source.size());

    Lexer lexer(std::move(source));

    // Stage: lex only
    if (stage == Stage::Lex)
    {
        bool hasError = false;
        while (true)
        {
            Token token = lexer.nextToken();
            if (token.type == TokenType::Error)
            {
                std::cerr << "[error] " << token.line << ":" << token.column
                          << " - " << token.lexeme << std::endl;
                hasError = true;
            }
            if (token.type == TokenType::EndOfFile)
                break;
        }
        return hasError ? 0 : 0;
    }

    // Stage: parse (lexer + parser)
    Parser parser(lexer);
    ParseResult result = parser.parse();

    for (const auto &d : result.diagnostics)
    {
        const char *severity = (d.severity == DiagnosticSeverity::Error) ? "error" : "warning";
        std::cerr << "[" << severity << "] "
                    << d.span.start.line << ":" << d.span.start.column
                    << " - " << d.message << std::endl;
    }

    if (result.status == ParseStatus::Aborted || parser.hasFatalError())
    {
        DEBUG_LOG(LogModule::Main, LogLevel::Error, "Parsing aborted for: %s", inputPath);
        return 1;
    }

    if (result.status == ParseStatus::RecoveredWithErrors)
    {
        DEBUG_LOG(LogModule::Main, LogLevel::Warn, "Parsing completed with errors for: %s", inputPath);
        return 0;
    }

    DEBUG_LOG(LogModule::Main, LogLevel::Info, "Parsing complete for: %s", inputPath);
    return 0;
}
