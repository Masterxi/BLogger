// Main BLogger header

#pragma once

/* Base BLogger class.
   Responsible for message formatting
   and main logging functions.
*/ 
#include "Loggers/BaseLogger.h"

/* Blocking version of BLogger.
   All logging happens on a
   single thread.
*/
#include "Loggers/BlockingLogger.h"

/* Async version of BLogger.
   Logs the mesages using
   a dedicated threadpool
   and a message queue.
*/
#include "Loggers/AsyncLogger.h"

// ---- Convenient typedefs ----
typedef BLogger::BlockingLogger              BlockingLogger;
typedef BLogger::AsyncLogger                 AsyncLogger;
typedef std::shared_ptr<BlockingLogger>      BlockingLoggerPtr;
typedef std::shared_ptr<AsyncLogger>         AsyncLoggerPtr;
typedef std::shared_ptr<BLogger::BaseLogger> BLoggerPtr;

// ---- BLogger logger properties struct ----
// Used for customizing the logger.
struct BLoggerProps
{
    bool async;

    bool console_logger;
    bool colored;
    BLoggerString tag;
    BLoggerString pattern;
    level filter;

    bool file_logger;
    BLoggerString path;
    size_t bytes_per_file;
    size_t log_files;
    bool rotate_logs;

    BLoggerProps()
        : async(true),
        console_logger(true),
        colored(true),
        tag(BLOGGER_WIDEN_IF_NEEDED("Unnamed")),
        pattern(BLOGGER_WIDEN_IF_NEEDED("")),
        filter(level::trace),
        file_logger(false),
        path(BLOGGER_WIDEN_IF_NEEDED("")),
        bytes_per_file(BLOGGER_INFINITE),
        log_files(0),
        rotate_logs(true)
    {
    }
};

// ---- BLogger factory ----
class CreateLogger
{
public:
    static BLoggerPtr FromProps(BLoggerProps& props)
    {
        BLoggerPtr out_logger;

        if (props.async)
        {
            // 'magic statics'
            BLogger::StdoutSink::GetGlobalWriteLock();
            BLogger::Formatter::timestamp_format();
            BLogger::Formatter::overflow_postfix();
            BLogger::Formatter::max_length();
            BLogger::thread_pool::get();

            out_logger = std::make_shared<AsyncLogger>(
                props.tag,
                props.filter,
                props.pattern.empty()
            );
        }
        else
            out_logger = std::make_shared<BlockingLogger>(
                props.tag,
                props.filter,
                props.pattern.empty()
            );

        if (!props.pattern.empty())
            out_logger->SetPattern(props.pattern);

        if (props.tag.empty()) props.tag = BLOGGER_WIDEN_IF_NEEDED("Unnamed");

        if (props.console_logger)
        {
            if (props.colored)
                out_logger->AddSink(
                    std::make_unique<BLogger::ColoredStdoutSink>()
                );
            else
                out_logger->AddSink(
                    std::make_unique<BLogger::StdoutSink>()
                );
        }

        if (props.file_logger)
        {
            if (!props.path.empty())
            {
                out_logger->AddSink(
                    std::make_unique<BLogger::FileSink>(
                        props.path, props.tag,
                        props.bytes_per_file,
                        props.log_files,
                        props.rotate_logs
                    )
                );
            }
        }

        return out_logger;
    }

    static BLoggerPtr AsyncConsole(
        BLoggerInString tag,
        level lvl,
        bool default_pattern = true,
        bool colored = true
    )
    {
        // 'magic statics'
        BLogger::StdoutSink::GetGlobalWriteLock();
        BLogger::Formatter::timestamp_format();
        BLogger::Formatter::overflow_postfix();
        BLogger::Formatter::max_length();
        BLogger::thread_pool::get();

        BLoggerPtr out_logger = 
            std::make_shared<AsyncLogger>(
                tag,
                lvl,
                default_pattern
            );

        if (colored)
            out_logger->AddSink(
                std::make_unique<BLogger::ColoredStdoutSink>()
            );
        else
            out_logger->AddSink(
                std::make_unique<BLogger::StdoutSink>()
            );

        return out_logger;
    }

    static BLoggerPtr BlockingConsole(
        BLoggerInString tag,
        level lvl,
        bool default_pattern = true,
        bool colored = true
    )
    {
        BLoggerPtr out_logger =
            std::make_shared<BlockingLogger>(
                tag,
                lvl,
                default_pattern
            );

        if (colored)
            out_logger->AddSink(
                std::make_unique<BLogger::ColoredStdoutSink>()
            );
        else
            out_logger->AddSink(
                std::make_unique<BLogger::StdoutSink>()
            );

        return out_logger;
    }
};

#undef BLOGGER_ARG_CLOSING
#undef BLOGGER_ARG_FULL
#undef BLOGGER_ARG_OPENING
#undef BLOGGER_DEFAULT_PATTERN
#undef BLOGGER_LVL_PATTERN
#undef BLOGGER_TS_PATTERN
#undef BLOGGER_MSG_PATTERN
#undef BLOGGER_TAG_PATTERN
#undef BLOGGER_TIMESTAMP_FORMAT
#undef BLOGGER_FILEMODE
#undef BLOGGER_FILE_WRITE
#undef BLOGGER_FORMAT_STRING
#undef BLOGGER_STD_TO_STRING
#undef BLOGGER_TIME_TO_STRING
#undef BLOGGER_STRING_LENGTH
#undef BLOGGER_TO_STRING
#undef BLOGGER_STACK_ALLOC
#undef BLOGGER_MEMORY_COPY
#undef BLOGGER_MEMORY_MOVE
#undef BLOGGER_OPEN_FILE
#undef BLOGGER_OVERFLOW_POSTFIX
#undef BLOGGER_FOR_EACH_DO
#undef BLOGGER_TERMINATE_WITH
#undef BLOGGER_TRUE_SIZE
#undef BLOGGER_UPDATE_TIME
#undef BLOGGER_INIT_UNICODE_MODE
