#pragma once

#include "WCore.h"


class WActor : public WObject
{
public:
    WActor() = default;
    virtual ~WActor() = default;
    
private:
    WId id_;
    std::string name_;
    

};