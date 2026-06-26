#pragma once

#include <string_view>
#include <cstring>
#include <cstdint>


template<std::uint32_t BufferSize=256>
struct TName{
public:

    TName() = default;

    TName(const TName& other) :
        size(other.size) {
        std::memcpy(data, other.data, size);
    }

    TName& operator=(const TName& other) {
        if (this != &other)
        {
            size = other.size;
            std::memcpy(data, other.data, size);
        }
        return *this;
    }

    TName(TName&& other) :
        size(other.size)
        {
            std::memcpy(data, other.data, size);
            other.size = 0;
        }
    
    TName& operator=(TName&& other) {
        if (this != &other) {
            size = other.size;
            std::memcpy(data, other.data, size);
            
            other.size=0;
        }
        return *this;
    }

    virtual ~TName() = default;

    TName(std::string_view in_name)
    {
        size = in_name.size();
        std::memcpy(data, in_name.data(), size);
    }

    TName(const char * in_name) :
        TName(std::string_view(in_name)){}

    std::string_view View() const {
        return {data, size};
    }

public:

    bool operator==(const TName & other) const {
        return size == other.size &&
            std::memcmp(data, other.data, size);
    }

private:

    char data[BufferSize]{};
    std::uint32_t size{0};
    
};

using WObjectName = TName<256>;

