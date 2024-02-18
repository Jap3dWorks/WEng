#include "WCore/WCore.h"
#include "WLog.h"


WClass::WClass(const char *name) : name_(name)
{
}

const char* WClass::GetName() const
{
    return name_;
}