#include "WCore.h"
#include "WLog.h"
#include "WAsset.h"


WClass::WClass(const char *name) : name_(name)
{
}

const char* WClass::GetName() const
{
    return name_;
}