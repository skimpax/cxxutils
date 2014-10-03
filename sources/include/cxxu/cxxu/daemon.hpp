#ifndef __CXXU_DAEMON_H__
#define __CXXU_DAEMON_H__

#include <string>

#include <timevault/config.h>

namespace cxxu {

class TIMEVAULT_API daemon
{
public:
    daemon(const std::string& name);
    virtual ~daemon();

    void operator()();

private:
    virtual void init();
    virtual void run() = 0;
    virtual void reconfig();
    virtual void quit() = 0;
    virtual void user1();
    virtual void user2();

    std::string name_;
};

} // namespace cxxu

#endif // __CXXU_DAEMON_H__
