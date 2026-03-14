#ifdef ENABLE_DEBUG_LOG

#include "debug.h"

#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <string>
#include <unordered_map>
#include <filesystem>

// ----------------------------------
// Module name and file path mapping
// ----------------------------------
static const char *moduleName(LogModule module)
{
    switch (module)
    {
    case LogModule::Main:      return "main";
    case LogModule::Lexer:     return "lexer";
    case LogModule::Parser:    return "parser";
    case LogModule::Semantic:  return "semantic";
    case LogModule::IR:        return "ir";
    case LogModule::Optimizer: return "optimizer";
    case LogModule::Codegen:   return "codegen";
    case LogModule::Runtime:   return "runtime";
    default:                   return "unknown";
    }
}

static const char *levelName(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Debug: return "DEBUG";
    case LogLevel::Info:  return "INFO";
    case LogLevel::Warn:  return "WARN";
    case LogLevel::Error: return "ERROR";
    default:              return "???";
    }
}

// ----------------------------------
// File handle cache (one per module)
// ----------------------------------
static std::unordered_map<LogModule, FILE *> logFiles;

static FILE *getLogFile(LogModule module)
{
    auto it = logFiles.find(module);
    if (it != logFiles.end())
        return it->second;

    // ensure logs/ directory exists
    std::filesystem::create_directories("logs");

    std::string path = std::string("logs/") + moduleName(module) + ".log";
    FILE *fp = std::fopen(path.c_str(), "a");
    if (!fp)
    {
        std::fprintf(stderr, "[DEBUG] Failed to open log file: %s\n", path.c_str());
        return nullptr;
    }

    logFiles[module] = fp;
    return fp;
}

// ----------------------------------
// Cleanup: flush and close all log files at exit
// ----------------------------------
static struct LogCleaner
{
    ~LogCleaner()
    {
        for (auto &[mod, fp] : logFiles)
        {
            if (fp)
                std::fclose(fp);
        }
        logFiles.clear();
    }
} logCleaner;

// ----------------------------------
// Core logging function
// ----------------------------------
void debug_log(LogModule module, LogLevel level, const char *file, int line, const char *fmt, ...)
{
    FILE *fp = getLogFile(module);
    if (!fp)
        return;

    // timestamp
    std::time_t now = std::time(nullptr);
    char timebuf[32];
    std::strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    // header: [2026-03-14 12:00:00] [INFO] [lexer.cpp:42]
    std::fprintf(fp, "[%s] [%-5s] [%s:%d] ", timebuf, levelName(level), file, line);

    // formatted message
    va_list args;
    va_start(args, fmt);
    std::vfprintf(fp, fmt, args);
    va_end(args);

    std::fprintf(fp, "\n");
    std::fflush(fp);
}

#endif // ENABLE_DEBUG_LOG
