#pragma once

#include "WCore.h"


class WCORE_API WActor : public WObject
{
public:
    WActor() = default;
    virtual ~WActor() = default;
    
private:
    std::string name_;
};
