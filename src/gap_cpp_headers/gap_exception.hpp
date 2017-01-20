#ifndef GAP_EXCEPTION_AJIFDA
#define GAP_EXCEPTION_AJIFDA

#include <exception>
#include <string>

struct GAPException : public std::runtime_error
{
    GAPException(const std::string& s)
    : std::runtime_error(s)
    { }
};

#endif
