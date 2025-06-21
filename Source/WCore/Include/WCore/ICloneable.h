#pragma once

#include <memory>

template<typename T>
class ICloneable {

public:

    virtual  std::unique_ptr<T> clone()=0;
  
};

