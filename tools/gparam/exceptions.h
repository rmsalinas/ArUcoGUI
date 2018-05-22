#ifndef _Exceptions_GPARAM_h
#define _Exceptions_GPARAM_h
#include <string>
#include <fstream> 
#include "exports.h"
#include <cstdarg>

namespace gparam {

/**
 * Types of valid exceptions
 */
class GPARAM_API Exceptions {
public:
    static const int InvalidParam=0;
    static const int OutOfBounds=1;
    static const int InputError=2;
    static const int FileNotFound=3;

};




/*!
 The standard GUSFSL_EXPORTS exception class.
 */
class GPARAM_API Exception : public std::exception
{
public:
    /*!
     Default constructor
     */
    Exception() {
        code = 0;
        line = 0;
    }
    /*!
     Full constructor. Normally the constuctor is not called explicitly.
     Instead, the macros CV_Error(), CV_Error_() and CV_Assert() are used.
    */
    Exception(int _code, const std::string& _err, const std::string& _func, const std::string& _file, int _line)
            : code(_code), err(_err), func(_func), file(_file), line(_line)
    {
        formatMessage();
    }
    virtual ~Exception() throw() {}

    /*!
     \return the error description and the context as a text string.
    */
    virtual const char *what() const throw() {
        return msg.c_str();
    }
    void formatMessage()
    {
        if ( func.size() > 0 )
            msg = format("%s:%d: error: (%d) %s in function %s\n", file.c_str(), line, code, err.c_str(), func.c_str());
        else
            msg = format("%s:%d: error: (%d) %s\n", file.c_str(), line, code, err.c_str());
    }

    std::string msg; ///< the formatted error message

    int code; ///< error code @see CVStatus
    std::string err; ///< error description
    std::string func; ///< function name. Available only when the compiler supports __func__ macro
    std::string file; ///< source file name where the error has occured
    int line; ///< line number in the source file where the error has occured

private:
    std::string format( const char* fmt, ... )
    {
        char buf[1 << 16];
        va_list args;
        va_start( args, fmt );
        vsprintf( buf, fmt, args );
        return std::string(buf);
    }
};

};
#endif
