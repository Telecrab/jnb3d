#include "calllogger.h"

#include <fstream>

std::once_flag CallLogger::m_onlyOne;
std::shared_ptr<CallLogger> CallLogger::m_instance = nullptr;

CallLogger::~CallLogger()
{
    std::ofstream out("callLog.txt", std::ios::out | std::ios::trunc);
    flush(out);
}

CallLogger &CallLogger::getInstance()
{
    std::call_once( CallLogger::m_onlyOne,
                    [=](){
                            CallLogger::m_instance = std::make_shared<CallLogger>( CallLogger::Key() );
                         });

    return *CallLogger::m_instance;
}

// http://stackoverflow.com/a/29856690
std::string CallLogger::computeMethodName(const std::string& function, const std::string& prettyFunction) {
    size_t locFunName = prettyFunction.find(function); //If the input is a constructor, it gets the beginning of the class name, not of the method. That's why later on we have to search for the first parenthesys
    size_t begin = prettyFunction.rfind(" ",locFunName) + 1;
    size_t end = prettyFunction.find("(",locFunName + function.length()); //Adding function.length() make this faster and also allows to handle operator parenthesys!
    if (prettyFunction[end + 1] == ')')
        return (prettyFunction.substr(begin,end - begin) + "()");
    else
        return (prettyFunction.substr(begin,end - begin) + "(...)");
}

void CallLogger::doLogCall(const std::string& name, const std::string& prettyName)
{
    //qDebug() << name;
    auto iterator = m_calledFunctions.find(prettyName);
    if ( iterator != m_calledFunctions.end() ) {
        m_callOrder.push_back( iterator->second );

    } else {
        m_prettyNames.push_back(prettyName);

        std::vector<std::string>::size_type functionIndex = m_prettyNames.size() - 1;
        m_callOrder.push_back(functionIndex);
        m_calledFunctions[prettyName] = functionIndex;
        m_indexToName[functionIndex] = name;
    }
}

void CallLogger::doFlush(std::ostream &out)
{
    if ( m_prettyNames.empty() ) return;

    out << "@startuml\n" << std::endl;

    for (std::vector<std::string>::size_type index = 0; index < m_callOrder.size() - 1; index++) {
        out << "\"" << m_indexToName[ m_callOrder[index] ] << "\"" << " -> " << "\"" << m_indexToName[ m_callOrder[index + 1] ] << "\"" << std::endl;
    }

    for (std::vector<std::string>::size_type index = 0; index < m_prettyNames.size(); index++) {
        out << "\n" << "note left of \"" << m_indexToName[0] << "\"" << std::endl;

        out << "    " << m_indexToName[index] << "\n    " << m_prettyNames[index] << std::endl;

        out << "end note" << std::endl;
    }

    out << "\n" << "@enduml" << std::endl;
}

void CallLogger::doClear()
{
    m_prettyNames.clear();
    m_calledFunctions.clear();
    m_callOrder.clear();
    m_indexToName.clear();
}
