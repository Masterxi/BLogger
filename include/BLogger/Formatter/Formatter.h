#pragma once

#include "BLogger/LogLevels.h"
#include "FormatUtilities.h"
#include "BLogger/OS/Functions.h"

#define BLOGGER_BUFFER_SIZE 128
#define BLOGGER_TS_PATTERN "%H:%M:%S"
#define BLOGGER_ARG_PATTERN "{}"

namespace BLogger
{
    typedef char charT;

    template<size_t size>
    using internal_buffer = std::array<charT, size>;

    typedef internal_buffer<BLOGGER_BUFFER_SIZE>
        BLoggerBuffer;

    template<typename bufferT>
    class blogger_basic_formatter
    {
    protected:
        bufferT    m_Buffer;
        charT*     m_Cursor;
        size_t     m_Occupied;
    public:
        blogger_basic_formatter()
            : m_Buffer(),
            m_Cursor(m_Buffer.data()),
            m_Occupied(0)
        {
        }

        void init_timestamp()
        {
            write_to_enclosed(
                BLOGGER_TS_PATTERN,
                std::strlen(BLOGGER_TS_PATTERN)
            );
        }

        template<typename T>
        void handle_pack(T&& arg)
        {
            std::stringstream ss;

            *this << *dynamic_cast<std::stringstream*>(&(ss << std::forward<T&&>(arg)));
        }
        
        template<typename... Args>
        void finalize_pack(Args&& ... args)
        {
            charT format[BLOGGER_BUFFER_SIZE];
            MEMORY_COPY(format, BLOGGER_BUFFER_SIZE, m_Buffer.data(), m_Buffer.size());

            size_t ts_offset = std::strlen(BLOGGER_TS_PATTERN) + 2;

            auto written = ts_offset +  
                snprintf(
                    (m_Buffer.data() + ts_offset),
                    (m_Buffer.size() - ts_offset),
                    (format + ts_offset),
                    args...
                );

            m_Occupied = ts_offset + written;
            m_Cursor = m_Buffer.data() + m_Occupied;
        }

        void add_space()
        {
            if ((m_Buffer.size() - m_Occupied) >= 1)
            {
                *(m_Cursor++) = ' ';
                m_Occupied += 1;
            }
        }

        void append_level(level lvl)
        {
            write_to_enclosed(
                LevelToString(lvl),
                std::strlen(LevelToString(lvl))
            );
        }

        void append_tag(const std::string& tag)
        {
            write_to_enclosed(
                tag.c_str(),
                tag.size()
            );
        }

        void newline()
        {
            if (!remaining())
            {
                --m_Cursor;
                *(m_Cursor++) = '\n';
            }
            else
            {
                m_Occupied += 1;
                *(m_Cursor++) = '\n';
            }
        }

        int32_t remaining()
        {
            return static_cast<int32_t>(m_Buffer.size()) -
                   static_cast<int32_t>(m_Occupied);
        }

        charT* data()
        {
            return m_Buffer.data();
        }

        charT* cursor()
        {
            return m_Cursor;
        }

        size_t size()
        {
            return m_Occupied;
        }

        void advance_cursor_by(size_t count)
        {
            m_Cursor += count;
            m_Occupied += count;
        }

        BLoggerBuffer& get_buffer()
        {
            return m_Buffer;
        }

        BLoggerBuffer&& release_buffer()
        {
            return std::move(m_Buffer);
        }

        void reset_buffer()
        {
            m_Occupied = 0;
            m_Cursor = m_Buffer.data();
            memset(m_Buffer.data(), 0, m_Buffer.size());
        }

        void write_to(const charT* data, size_t size)
        {
            if ((m_Buffer.size() - m_Occupied) >= size)
            {
                MEMORY_COPY(m_Cursor, remaining(), data, size);
                m_Cursor += size;
                m_Occupied += size;
            }
        }
    private:
        blogger_basic_formatter<bufferT>& operator<<(std::stringstream& ss)
        {
            charT* cursor = get_next_arg();

            if (!cursor) return *this;

            *cursor = '%';
            *(cursor + 1) = 's';

            charT format[BLOGGER_BUFFER_SIZE];
            MEMORY_COPY(format, BLOGGER_BUFFER_SIZE, m_Buffer.data(), m_Buffer.size());

            size_t ts_offset = std::strlen(BLOGGER_TS_PATTERN) + 2;

            auto written = ts_offset +
                snprintf(
                (m_Buffer.data() + ts_offset),
                    (m_Buffer.size() - ts_offset),
                    (format + ts_offset),
                    ss.str().c_str()
                );

            m_Occupied = written;
            m_Cursor = m_Buffer.data() + m_Occupied;

            return *this;
        }

        void write_to_enclosed(const charT* data, size_t size, charT opening = '[', charT closing = ']')
        {
            if ((m_Buffer.size() - m_Occupied) >= size + 2)
            {
                *(m_Cursor++) = opening;
                ++m_Occupied;
                MEMORY_COPY(m_Cursor, remaining(), data, size);
                m_Occupied += size;
                m_Cursor += size;
                *(m_Cursor++) = closing;
                ++m_Occupied;
            }
        }

        charT* get_next_arg()
        {
            auto index = std::search(
                m_Buffer.begin(),
                m_Buffer.end(),
                BLOGGER_ARG_PATTERN,
                BLOGGER_ARG_PATTERN +
                std::strlen(BLOGGER_ARG_PATTERN)
            );

            return (index != m_Buffer.end() ? &(*index) : nullptr);
        }
    };

    typedef blogger_basic_formatter<BLoggerBuffer>
        BLoggerFormatter;
}

#undef BLOGGER_BUFFER_SIZE
#undef BLOGGER_ARG_PATTERN