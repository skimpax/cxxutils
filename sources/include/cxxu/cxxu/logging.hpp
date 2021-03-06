#ifndef __CXXU_LOGGING_H__
#define __CXXU_LOGGING_H__

#include <string>
#include <sstream>
#include <stdexcept>
#include <system_error>
#include <chrono>

#include <cxxutils/config.h>

#include <cxxu/utils.hpp>
#include <cxxu/exceptions.hpp>
#include <cxxu/logging/log_writer.hpp>

namespace cxxu {

class CXXUTILS_API log_item : public std::ostringstream
{
public:
    log_item(
        logging::log_writer_ptr writer_ptr,
        logging::level level = logging::level::none
    );
    log_item(log_item&& other);
    ~log_item();

private:
    logging::log_writer& writer();

    logging::log_writer_ptr writer_ptr_;
    logging::level level_;
};

class CXXUTILS_API timed_log_item : public log_item
{
public:
    timed_log_item(logging::log_writer_ptr writer_ptr);
    timed_log_item(logging::log_writer_ptr writer_ptr, const std::string& label);
    timed_log_item(timed_log_item&& other);
    ~timed_log_item();

private:
    std::chrono::time_point<std::chrono::system_clock> start_;
    std::chrono::time_point<std::chrono::system_clock> end_;
};

class CXXUTILS_API logger
{
public:
    static logger& get();

    void daemon(bool val, const std::string& name = "");

    log_item log();
    log_item info();
    log_item warning();
    log_item error();
    log_item system_error();
    log_item die();
    timed_log_item timed();
    timed_log_item timed(const std::string& label);

private:
    logger();
    ~logger();

    template <typename Item, typename... Args>
    Item make_item(Args&&... args)
    { return Item(writer_ptr_, std::forward<Args>(args)...); }

    logger(const logger&) = delete;
    void operator=(const logger&) = delete;

    logging::log_writer_ptr writer_ptr_;
};

inline
log_item log()
{ return logger::get().log(); }

inline
log_item info()
{ return logger::get().info(); }

inline
log_item warning()
{ return logger::get().warning(); }

inline
log_item error()
{ return logger::get().error(); }

inline
log_item system_error()
{ return logger::get().system_error(); }

inline
log_item die()
{ return logger::get().die(); }

inline
timed_log_item timed()
{ return logger::get().timed(); }

inline
timed_log_item timed(const std::string& label)
{ return logger::get().timed(label); }

} // namespace cxxu

#define CXXU_THROW(WHAT)                                                      \
    do {                                                                      \
        std::ostringstream o__;                                               \
        o__ << WHAT;                                                          \
        throw cxxu::exception(o__.str());                                     \
    } while (0);

#define CXXU_DIE_BEGIN(WHAT, TYPE)                                            \
    do {                                                                      \
        std::string s__;                                                      \
        {                                                                     \
            cxxu::log_item l__ = cxxu::TYPE();                                \
            l__ << WHAT;                                                      \
            s__ = l__.str();                                                  \
        }

#define CXXU_DIE_END()                                                        \
    } while (0);

#define CXXU_DIE(WHAT)                                                        \
    CXXU_DIE_BEGIN(WHAT, die)                                                 \
        throw std::runtime_error(s__);                                        \
    CXXU_DIE_END()

#define CXXU_DIE_IF(COND, WHAT)                                               \
    if (COND) {                                                               \
        CXXU_DIE(WHAT)                                                        \
    }

#define CXXU_DIE_UNLESS(COND, WHAT)                                           \
    if (!(COND)) {                                                            \
        CXXU_DIE(WHAT)                                                        \
    }

#define CXXU_SYSDIE(WHAT)                                                     \
    CXXU_DIE_BEGIN(WHAT, die)                                                 \
        throw std::system_error(errno, std::system_category(), s__);          \
    CXXU_DIE_END()

#define CXXU_SYSDIE_IF(COND, WHAT)                                            \
    if (COND) {                                                               \
        CXXU_SYSDIE(WHAT)                                                     \
    }

#define CXXU_SYSDIE_UNLESS(COND, WHAT)                                        \
    if (!(COND)) {                                                            \
        CXXU_SYSDIE(WHAT)                                                     \
    }

#endif // __CXXU_LOGGING_H__
