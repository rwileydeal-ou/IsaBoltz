#ifndef NotImplementedException_h
#define NotImplementedException_h

#include <stdexcept>

class NotImplementedException : public std::logic_error
{
public:
    NotImplementedException() : std::logic_error("Function not yet implemented") { };
};

#endif
