#pragma once

#include <cstdarg>
#include <cstdio>

// ----------------------------------
// Log levels
// ----------------------------------
enum class LogLevel
{
    Debug,
    Info,
    Warn,
    Error
};

// ----------------------------------
// Log modules (one log file per module)
// ----------------------------------
enum class LogModule
{
    Main,
    Lexer,
    Parser,
    Semantic,
    IR,
    Optimizer,
    Codegen,
    Runtime
};

// ----------------------------------
// Debug log macro
// When ENABLE_DEBUG_LOG is defined, DEBUG_LOG calls debug_log().
// Otherwise it compiles to nothing.
// ----------------------------------
#ifdef ENABLE_DEBUG_LOG

void debug_log(LogModule module, LogLevel level, const char *file, int line, const char *fmt, ...);

#define DEBUG_LOG(module, level, fmt, ...) \
    debug_log(module, level, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

// ----------------------------------
// RAII trace helper: logs Entering/Exiting automatically
// Usage: TRACE_ENTER_EXIT(LogModule::Lexer);
// ----------------------------------
struct TraceScope
{
    LogModule module;
    const char *func;
    const char *file;
    int line;

    TraceScope(LogModule m, const char *fn, const char *f, int l)
        : module(m), func(fn), file(f), line(l)
    {
        debug_log(module, LogLevel::Info, file, line, "Entering %s()...", func);
    }

    ~TraceScope()
    {
        debug_log(module, LogLevel::Info, file, line, "Exiting %s()...", func);
    }
};

#define TRACE_ENTER_EXIT(module) \
    TraceScope _trace_scope_(module, __func__, __FILE__, __LINE__)

// ----------------------------------
// Readable char representation for logging
// Converts control characters to their escape names.
// Usage: DEBUG_LOG(..., "char = %s", charRepr(c));
// ----------------------------------
inline const char *charRepr(char c)
{
    static char buf[8];
    switch (c)
    {
    case '\0': return "'\\0'";
    case '\n': return "'\\n'";
    case '\t': return "'\\t'";
    case '\r': return "'\\r'";
    case ' ':  return "' '";
    default:
        std::snprintf(buf, sizeof(buf), "'%c'", c);
        return buf;
    }
}

#else

#define DEBUG_LOG(module, level, fmt, ...) ((void)0)
#define TRACE_ENTER_EXIT(module) ((void)0)

#endif
