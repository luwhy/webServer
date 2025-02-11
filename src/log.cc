#include "log.h"
#include <functional>
#include <map>
#include <time.h>
#include <string.h>
#include "config.h"
namespace sylar
{

    const char *
    LogLevel::ToString(LogLevel::Level Level)
    {
        switch (Level)
        {
#define XX(name)                \
    case LogLevel::Level::name: \
        return #name;           \
        break;
            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);
            XX(FATAL);
#undef XX
        default:
            return "UNKONW";
            break;
        }
    }

    LogLevel::Level LogLevel::FromString(const std::string &str)
    {
#define XX(level, v)            \
    if (str == #v)              \
    {                           \
        return LogLevel::level; \
    }
        XX(DEBUG, debug);
        XX(INFO, info);
        XX(WARN, warn);
        XX(ERROR, error);
        XX(FATAL, fatal);

        XX(DEBUG, DEBUG);
        XX(INFO, INFO);
        XX(WARN, WARN);
        XX(ERROR, ERROR);
        XX(FATAL, FATAL);
        return LogLevel::UNKOWN;
#undef XX
    }

    class MessageFormatItem : public FormatItem
    {
    public:
        MessageFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getContent();
        }
    };

    class LevelFormatItem : public FormatItem
    {
    public:
        LevelFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << LogLevel::ToString(level);
        }
    };

    class ElapseFormatItem : public FormatItem
    {
    public:
        ElapseFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getElapse();
        }
    };

    class NameFormatItem : public FormatItem
    {
    public:
        NameFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getLogger()->getName();
        }
    };

    class ThreadIdFormatItem : public FormatItem
    {
    public:
        ThreadIdFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getThreadId();
        }
    };

    class DateTimeFormatItem : public FormatItem
    {
    public:
        DateTimeFormatItem(const std::string &format = "%Y-%m-%d %H:%M:%S") : m_format(format)
        {
            if (m_format.empty())
            {
                m_format = "%Y-%m-%d %H:%M:%S";
            }
        }
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            struct tm _tm;
            time_t time = event->getTime();
            localtime_r(&time, &_tm);
            char buf[64];
            strftime(buf, sizeof(buf), m_format.c_str(), &_tm);
            os << buf;
        }

    private:
        std::string m_format;
    };

    class FilenameFormatItem : public FormatItem
    {
    public:
        FilenameFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getFile();
        }
    };

    // 线程名称
    class ThreadNameFormatterItem : public FormatItem
    {
    public:
        ThreadNameFormatterItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getThreadName();
        }
    };

    class LineFormatItem : public FormatItem
    {
    public:
        LineFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getline();
        }
    };

    class StringFormatItem : public FormatItem
    {
    public:
        StringFormatItem(const std::string &str) : FormatItem(str), m_string(str) {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << m_string;
        }

    private:
        std::string m_string;
    };

    class TabFormatItem : public FormatItem
    {
    public:
        TabFormatItem(const std::string str = "\t") : FormatItem(str), m_string(str) {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << "\t";
        }

    private:
        std::string m_string;
    };

    class NewLineFormatItem : public FormatItem
    {
    public:
        NewLineFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << std::endl;
        }
    };

    LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char *file, int32_t line, uint32_t elapse, uint32_t thread_id, uint64_t time, const std::string &threadName)
    {
        this->m_logger = logger;
        this->m_file = file;
        this->m_line = line;
        this->m_elapse = elapse;
        this->m_threadId = thread_id;
        this->m_time = time;
        this->m_level = level;
        this->m_threadName = threadName;
    }

    void LogEvent::format(const char *fmt, va_list al)
    {
        char *buf = nullptr;
        int len = vasprintf(&buf, fmt, al);
        if (len != -1)
        {
            m_ss << std::string(buf, len);
            free(buf);
        }
    }

    void LogEvent::format(const char *fmt, ...)
    {
        va_list al;
        va_start(al, fmt);
        format(fmt, al);
        va_end(al);
    }

    Logger::Logger(const std::string &name) : m_name(name), m_level(LogLevel::Level::DEBUG)
    {

        m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T[%p]%T[%c]%T%f:%l%T%m%n"));
    }

    void Logger::addAppender(LogAppender::ptr appender)
    {
        MutexType::Lock lock(m_mutex);
        if (!appender->getFormatter())
        {
            MutexType::Lock ll(appender->m_mutex);
            appender->m_formatter = m_formatter;
        }
        m_appenders.push_back(appender);
    }

    void Logger::delAppender(LogAppender::ptr appender)
    {
        MutexType::Lock lock(m_mutex);
        for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it)
        {
            if (*it == appender)
            {
                m_appenders.erase(it);
                break;
            }
        }
    }

    void Logger::clearAppenders()
    {
        MutexType::Lock lock(m_mutex);
        this->m_appenders.clear();
    }

    void Logger::setFormatter(LogFormatter::ptr val)
    {
        MutexType::Lock lock(m_mutex);
        this->m_formatter = val;
        for (auto &i : m_appenders)
        {
            MutexType::Lock ll(i->m_mutex);
            if (!i->m_hasFormatter)
            {
                i->m_formatter = m_formatter;
            }
        }
    }

    void Logger::setFormatter(const std::string &val)
    {
        sylar::LogFormatter::ptr new_val = std::make_shared<sylar::LogFormatter>(val);
        if (new_val->isError())
        {
            std::cout << "Logger setFormatter name= " << m_name << "value: " << val << " invalid formatter" << std::endl;
            return;
        }
        // this->m_formatter = new_val;
        setFormatter(new_val);
    }

    LogFormatter::ptr Logger::getFormatter()
    {
        MutexType::Lock lock(m_mutex);
        return this->m_formatter;
    }

    std::string Logger::toYamlString()
    {
        MutexType::Lock lock(m_mutex);
        YAML::Node node;
        node["name"] = m_name;
        node["level"] = LogLevel::ToString(m_level);
        if (m_formatter)
            node["formatter"] = m_formatter->getParttern();

        for (auto &i : m_appenders)
        {
            node["appenders"].push_back(YAML::Load(i->toYamlString()));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }

    // shared_from_this:获取自身shared_ptr,需要在类搭配td::enable_shared_from_this<Logger>使用
    void Logger::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            auto self = shared_from_this();
            MutexType::Lock lock(m_mutex);
            if (!m_appenders.empty())
            {
                for (auto &i : m_appenders)
                {
                    i->log(self, level, event);
                }
            }
            else if (m_root)
            {
                m_root->log(level, event);
            }
        }
    }

    void Logger::debug(LogEvent::ptr event)
    {
        log(LogLevel::Level::DEBUG, event);
    }
    void Logger::info(LogEvent::ptr event)
    {
        log(LogLevel::Level::INFO, event);
    }
    void Logger::warn(LogEvent::ptr event)
    {
        log(LogLevel::Level::WARN, event);
    }
    void Logger::error(LogEvent::ptr event)
    {
        log(LogLevel::Level::ERROR, event);
    }
    void Logger::fatal(LogEvent::ptr event)
    {
        log(LogLevel::Level::FATAL, event);
    }

    /**
     * LogAppender
     *
     */
    void LogAppender::setFormatter(LogFormatter::ptr val)
    {
        MutexType::Lock lock(m_mutex);
        m_formatter = val;
        if (m_formatter)
        {
            m_hasFormatter = true;
        }
        else
        {
            m_hasFormatter = false;
        }
    }

    LogFormatter::ptr LogAppender::getFormatter()
    {
        MutexType::Lock lock(m_mutex);
        return m_formatter;
    }

    /**
     *
     * FileLogAppender
     *
     */
    FileLogAppender::FileLogAppender(const std::string &filename) : m_filename(filename)
    {
        this->reopen();
    }

    bool FileLogAppender::reopen()
    {
        MutexType::Lock lock(m_mutex);
        if (m_filestream)
        {
            m_filestream.close();
        }
        m_filestream.open(m_filename);
        // 双感叹号转0为1  所以这里可以是!!m_filestream
        return m_filestream.is_open();
    }

    std::string FileLogAppender::toYamlString()
    {
        MutexType::Lock lock(m_mutex);
        YAML::Node node;
        node["type"] = "FileLogAppender";
        node["file"] = m_filename;
        if (m_level != LogLevel::UNKOWN)
        {
            node["level"] = LogLevel::ToString(m_level);
        }

        if (m_hasFormatter && m_formatter)
        {
            node["formatter"] = m_formatter->getParttern();
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }

    void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            uint64_t now = time(0);
            if (now != m_lastTime)
            {
                reopen();
                m_lastTime = now;
            }
            MutexType::Lock lock(m_mutex);
            if (m_filestream << m_formatter->format(logger, level, event))
            {
                std::cout << "error" << std::endl;
            }
        }
    }
    /**
     *
     * StdoutLogAppender
     *
     */

    void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            MutexType::Lock lock(m_mutex);
            std::cout << m_formatter->format(logger, level, event);
        }
    }

    std::string StdoutLogAppender::toYamlString()
    {
        MutexType::Lock lock(m_mutex);
        YAML::Node node;
        node["type"] = "StdoutLogAppender";
        if (m_level != LogLevel::UNKOWN)
        {
            node["level"] = LogLevel::ToString(m_level);
        }

        if (m_hasFormatter && m_formatter)
        {
            node["formatter"] = m_formatter->getParttern();
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }

    LogFormatter::LogFormatter(const std::string &pattern) : m_pattern(pattern)
    {
        init();
    }

    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        std::stringstream ss;
        for (auto &i : m_items)
        {
            i->format(ss, logger, level, event);
        }
        return ss.str();
    }
    void LogFormatter::init()
    {
        std::vector<std::tuple<std::string, std::string, int>> vec;
        std::string nstr;
        for (size_t i = 0; i < m_pattern.size(); ++i)
        {
            if (m_pattern[i] != '%')
            {
                nstr.append(1, m_pattern[i]);
                continue;
            }

            if ((i + 1) < m_pattern.size())
            {
                if (m_pattern[i + 1] == '%')
                {
                    nstr.append(1, '%');
                    continue;
                }
            }

            size_t n = i + 1;
            int fmt_status = 0;
            size_t fmt_begin = 0;

            std::string str;
            std::string fmt;
            while (n < m_pattern.size())
            {
                if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}'))
                {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    break;
                }
                if (fmt_status == 0)
                {
                    if (m_pattern[n] == '{')
                    {
                        str = m_pattern.substr(i + 1, n - i - 1);
                        // std::cout << "*" << str << std::endl;
                        fmt_status = 1; // 解析格式
                        fmt_begin = n;
                        ++n;
                        continue;
                    }
                }
                else if (fmt_status == 1)
                {
                    if (m_pattern[n] == '}')
                    {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        // std::cout << "#" << fmt << std::endl;
                        fmt_status = 0;
                        ++n;
                        break;
                    }
                }
                ++n;
                if (n == m_pattern.size())
                {
                    if (str.empty())
                    {
                        str = m_pattern.substr(i + 1);
                    }
                }
            }

            if (fmt_status == 0)
            {
                if (!nstr.empty())
                {
                    vec.push_back(std::make_tuple(nstr, std::string(), 0));
                    nstr.clear();
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n - 1;
            }
            else if (fmt_status == 1)
            {
                std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
                m_error = true;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
        }

        if (!nstr.empty())
        {
            vec.push_back(std::make_tuple(nstr, "", 0));
        }

        static std::map<std::string, std::function<FormatItem::ptr(const std::string &str)>> s_format_items = {
#define XX(str, C) \
    {              \
        #str, [](const std::string &fmt) { return FormatItem::ptr(new C(fmt)); }}
            XX(m, MessageFormatItem),      // m:消息
            XX(p, LevelFormatItem),        // p:日志级别
            XX(r, ElapseFormatItem),       // r:累计毫秒数
            XX(c, NameFormatItem),         // c:日志名称
            XX(t, ThreadIdFormatItem),     // t:线程id
            XX(n, NewLineFormatItem),      // n:换行
            XX(d, DateTimeFormatItem),     // d:时间
            XX(f, FilenameFormatItem),     // f:文件名
            XX(l, LineFormatItem),         // l:行号
            XX(T, TabFormatItem),          // T:Tab
            XX(N, ThreadNameFormatterItem) // XX(N, ThreadNameFormatItem), // N:线程名称

#undef XX
        };
        for (auto &i : vec)
        {
            if (std::get<2>(i) == 0)
            {
                m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
            }
            else
            {
                auto it = s_format_items.find(std::get<0>(i));
                if (it == s_format_items.end())
                {
                    m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                    m_error = true;
                }
                else
                {
                    m_items.push_back(it->second(std::get<1>(i)));
                }
            }

            // std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
        }
        //%m --消息体
        //%p --level
        //%r --启动后的时间
        //%c --日志名称
        //%t --线程id
        //%n --回车换行
        //%d --时间
        //%f --文件名
        //%l --行号
    }
    FormatItem::FormatItem(const std::string &fmt)
    {
    }

    // LogEventWrap类内函数定义
    LogEventWrap::LogEventWrap(LogEvent::ptr e)
    {
        this->m_event = e;
    }
    LogEventWrap::~LogEventWrap()
    {
        m_event->getLogger()->log(m_event->getLevel(), m_event);
    }
    std::stringstream &LogEventWrap::getSS()
    {
        // TODO: 在此处插入 return 语句
        return m_event->getSS();
    }

    LogEvent::ptr LogEventWrap::getEvent()
    {
        return m_event;
    }

    struct LogAppenderDefine
    {
        int type = 0; // 1是file，2是stdout
        LogLevel::Level level = LogLevel::Level::UNKOWN;
        std::string formatter;
        std::string file;
        bool operator==(const LogAppenderDefine &oth) const
        {
            return type == oth.type && level == oth.level && formatter == oth.formatter && file == oth.file;
        }
    };

    struct LogDefine
    {
        std::string name;
        LogLevel::Level level;
        std::string formatter;
        std::vector<LogAppenderDefine> appenders;
        bool operator==(const LogDefine &oth) const
        {
            return name == oth.name && level == oth.level && appenders == appenders;
        }
        bool operator<(const LogDefine &oth) const
        {
            return name < oth.name;
        }
    };

    template <>
    class LexicalCast<std::string, std::set<LogDefine>>
    {
    public:
        std::set<LogDefine> operator()(const std::string &v)
        {
            YAML::Node node = YAML::Load(v);
            std::set<LogDefine> vec;
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); ++i)
            {
                const auto &n = node[i];
                if (!n["name"].IsDefined())
                {
                    std::cout << "log config error: name is null, " << n << std::endl;
                    continue;
                }
                LogDefine ld;
                // if (n["name"].IsScalar())
                ld.name = n["name"].as<std::string>();
                ld.level = LogLevel::FromString(n["level"].IsDefined() ? n["level"].as<std::string>() : "");
                if (n["formatter"].IsDefined())
                {
                    ld.formatter = n["formatter"].as<std::string>();
                }
                if (n["appenders"].IsDefined())
                {
                    for (size_t x = 0; x < n["appenders"].size(); ++x)
                    {
                        auto a = n["appenders"][x];

                        if (!a["type"].IsDefined())
                        {
                            std::cout << "log config error: appender type is null, " << n << std::endl;
                            continue;
                        }
                        std::string type = a["type"].as<std::string>();
                        LogAppenderDefine lad;
                        if (type == "FileLogAppender")
                        {
                            lad.type = 1;
                            if (!a["file"].IsDefined())
                            {
                                std::cout << "log config error: fileappender file is null, " << n << std::endl;
                                continue;
                            }
                            lad.file = a["file"].as<std::string>();
                            if (a["formatter"].IsDefined())
                            {
                                lad.formatter = a["formatter"].as<std::string>();
                            }
                        }
                        else if (type == "StdoutLogAppender")
                        {
                            lad.type = 2;
                        }
                        else
                        {
                            std::cout << "log config error: appender type is invalid, " << n << std::endl;
                            continue;
                        }
                        ld.appenders.push_back(lad);
                    }
                }
                vec.insert(ld);
            }
            return vec;
        }
    };

    // 类转str
    template <>
    class LexicalCast<std::set<LogDefine>, std::string>
    {
    public:
        std::string operator()(const std::set<LogDefine> &v)
        {
            YAML::Node node;
            for (auto &i : v)
            {
                YAML::Node n;
                n["name"] = i.name;
                if (i.level != LogLevel::UNKOWN)
                {
                    n["level"] = LogLevel::ToString(i.level);
                }

                if (i.formatter.empty())
                {
                    n["level"] = i.formatter;
                }
                for (auto &a : i.appenders)
                {
                    YAML::Node na;

                    if (a.type == 1)
                    {
                        na["type"] = "FileLogAppender";
                        na["file"] = a.file;
                    }
                    else if (a.type == 2)
                    {
                        na["type"] = "StdoutLogAppender";
                    }

                    na["level"] = LogLevel::ToString(a.level);
                    if (!a.formatter.empty())
                    {
                        na["formatter"] = a.formatter;
                    }
                    n["appenders"].push_back(na);
                }
                node.push_back(n);
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    ConfigVar<std::set<LogDefine>>::ptr g_log_defines = Config::Lookup("logs", std::set<LogDefine>(), "logs config");

    struct LogIniter
    {
        LogIniter()
        {
            g_log_defines->addListener([](const std::set<LogDefine> &old_value, const std::set<LogDefine> &new_value)
                                       {
                                           SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "on_logger_conf_changed";
                                           for (auto &i : new_value)
                                           {
                                               auto it = old_value.find(i);
                                               sylar::Logger::ptr logger;
                                               if (it == old_value.end())
                                               {
                                                   // 新增logger
                                                   logger = SYLAR_LOG_NAME(i.name);
                                               }
                                               else
                                               {
                                                   if (!(i == *it))
                                                   {
                                                       // 修改logger
                                                       logger = SYLAR_LOG_NAME(i.name);
                                                   }
                                               }
                                               logger->setLevel(i.level);
                                               if (!i.formatter.empty())
                                               {
                                                   logger->setFormatter(i.formatter);
                                               }
                                               logger->clearAppenders();
                                               for (auto &a : i.appenders)
                                               {
                                                   sylar::LogAppender::ptr ap;
                                                   if (a.type == 1)
                                                   {
                                                       ap.reset(new FileLogAppender(a.file));
                                                   }
                                                   else if (a.type == 2)
                                                   {
                                                       ap.reset(new StdoutLogAppender);
                                                   }
                                                   ap->setLevel(a.level);
                                                   if (!a.formatter.empty())
                                                   {
                                                       LogFormatter::ptr fmt = std::make_shared<LogFormatter>(a.formatter);
                                                       if (!fmt->isError())
                                                       {
                                                           ap->setFormatter(fmt);
                                                       }
                                                       else
                                                       {
                                                           std::cout << "log.name " << a.type << " formatter " << a.formatter << " is invalid" << std::endl;
                                                       }
                                                   }
                                                   logger->addAppender(ap);
                                               }
                                           }
                                           for (auto &i : old_value)
                                           {
                                               auto it = new_value.find(i);
                                               if (it == new_value.end())
                                               {
                                                   // 删除logger
                                                   auto logger = SYLAR_LOG_NAME(i.name);
                                                   logger->setLevel((LogLevel::Level)100);
                                                   logger->clearAppenders();
                                               }
                                           }
                                           // 删除
                                       });
        }
    };
    static LogIniter __log_init;

    LogManage::LogManage()
    {
        m_root.reset(new Logger);
        m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
        m_loggers[m_root->m_name] = m_root;
        init();
    }
    // logger管理
    Logger::ptr LogManage::getLogger(const std::string &name)
    {
        MutexType::Lock lock(m_mutex);
        auto it = m_loggers.find(name);
        if (it != m_loggers.end())
        {
            return it->second;
        }
        Logger::ptr logger(new Logger(name));
        logger->m_root = m_root;
        m_loggers[name] = logger;
        return logger;
    }
    void LogManage::init()
    {
    }

    std::string LogManage::toYamlString()
    {
        MutexType::Lock lock(m_mutex);
        YAML::Node node;
        for (auto &i : m_loggers)
        {
            node.push_back(YAML::Load(i.second->toYamlString()));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }

}