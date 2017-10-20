#ifndef CALLLOGGER_H
#define CALLLOGGER_H

#include <memory>
#include <mutex>
#include <unordered_map>
#include <string>
#include <vector>
#include <ostream>

// http://silviuardelean.ro/2012/06/05/few-singleton-approaches/
class CallLogger
{
public:
    // https://anthony-arnold.com/2012/04/05/three-fun-cpp-techniques/
    class Key {
        friend class CallLogger;
        Key() {}
    };

    CallLogger() = delete;
    CallLogger(const CallLogger& rs) = delete;
    CallLogger& operator = (const CallLogger& rs) = delete;

    CallLogger(const Key &) {}
    ~CallLogger();

    static CallLogger& getInstance();

    static void logCall(const std::string& name, const std::string& prettyName)
    {
        if (!getInstance().m_isEnabled) return;

        getInstance().doLogCall( getInstance().computeMethodName(name, prettyName), prettyName );
    }

    static void flush(std::ostream& out) { getInstance().doFlush(out); }
    static void clear() { getInstance().doClear(); }
    static void enable() { getInstance().m_isEnabled = true; }
    static void disable() { getInstance().m_isEnabled = false; }

private:
    static std::shared_ptr<CallLogger> m_instance;
    static std::once_flag m_onlyOne;

    std::vector<std::string> m_prettyNames;
    std::vector< std::vector<std::string>::size_type > m_callOrder;
    std::unordered_map< std::string, std::vector<std::string>::size_type > m_calledFunctions;
    std::unordered_map< std::vector<std::string>::size_type, std::string > m_indexToName;
    bool m_isEnabled = false;

    std::string computeMethodName(const std::string &function, const std::string &prettyFunction);
    void doLogCall(const std::string& name, const std::string& prettyName);
    void doFlush(std::ostream& out);
    void doClear();
};

#define LOG_CALL CallLogger::logCall(__FUNCTION__, __PRETTY_FUNCTION__);

#endif // CALLLOGGER_H
