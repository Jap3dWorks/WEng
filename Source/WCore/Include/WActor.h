#pragma once

#include "WCore.h"


class WActor : public WObject
{
public:
    WActor() = default;
    virtual ~WActor() = default;
    
private:
    std::string name_;
};