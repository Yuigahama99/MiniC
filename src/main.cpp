#include <iostream>
#include <fstream>
#include <utility>

#include "common/debug.h"
#include "lexer/include/lexer.h"
#include "parser/include/parser.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open())
    {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::string source{std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>()};
    inputFile.close();

    DEBUG_LOG(LogModule::Main, LogLevel::Info, "Loaded source file: %s (%zu bytes)", argv[1], source.size());

    Lexer lexer(std::move(source));
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
        DEBUG_LOG(LogModule::Main, LogLevel::Error, "Parsing aborted for: %s", argv[1]);
        return 1;
    }

    if (result.status == ParseStatus::RecoveredWithErrors)
    {
        DEBUG_LOG(LogModule::Main, LogLevel::Warn, "Parsing completed with errors for: %s", argv[1]);
        return 0;
    }

    DEBUG_LOG(LogModule::Main, LogLevel::Info, "Parsing complete for: %s", argv[1]);
    return 0;
}
