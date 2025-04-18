#include <string>
#include <iostream>

class TraceLogger{

public:
    TraceLogger(const char* fileName, const char* funcName, int lineNumber) {
        _fileName = fileName;
        _funcName = funcName;
        std::cout << Indent << "Entering " << _funcName << "() - (" << _fileName << ":" << lineNumber << ")" << std::endl;
        Indent.append("  ");
    }
    
    ~TraceLogger() {
        Indent.resize(Indent.length() - 2);
        std::cout << Indent << "Leaving  " << _funcName << "() - (" << _fileName << ")" << std::endl;
    }
    
private:

    std::string Indent;
	const char* _fileName;
	const char* _funcName;
};

#ifdef DEBUG
#define LOG_TRACE TraceLogger logger(__FILE__, __FUNCTION__, __LINE__);
#else
#define LOG_TRACE
#endif