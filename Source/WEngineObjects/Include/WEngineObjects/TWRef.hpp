#pragma once

#include "WCore/TRef.hpp"
#include <unordered_map>
#include <vector>
#include <concepts>

class WObject;

template<typename T>
struct TWRefTrack_ {

    static void UnregInstnc(void * in_address, T * in_obj) {
        if (!in_obj) return;

        assert(Track()[in_address].contains(in_obj));

        // TODO erase elements

    }

    static void RegInstnc(void * in_address, T * in_obj) {
        if (!in_obj) return;
        
        if (!Track().contains(in_address)) {
            Track().insert(in_address, {});
            Track()[in_address].reserve(64);
        }

        Track()[in_address].push_back(in_obj);
    }

    static const std::vector<T*> & Instances(void * in_address) {
        assert(Track().contains(in_address));

        return Track()[in_address];
    }

private:    

    static std::unordered_map<void*, std::vector<T*>> & Track() noexcept {
        static std::unordered_map<void*, std::vector<T*>> track_{};
        return track_;
    }
};

class BWRef {
public:
    using TRACK = TWRefTrack_<BWRef>;

    constexpr BWRef() noexcept : object_(nullptr) {}

    constexpr BWRef(WObject * in_object) :
        object_(in_object) {
        TRACK::RegInstnc(Ptr(), this);
    }

    BWRef(const BWRef & other) :
        object_(other.object_) {
        TRACK::RegInstnc(Ptr(), this);
    }

    BWRef(BWRef && other) :
        object_(std::move(other.object_)) {
        TRACK::UnregInstnc(Ptr(), &other);
        TRACK::RegInstnc(Ptr(), this);
    }

    BWRef & operator=(const BWRef & other) {
        if (this != &other) {
            Set(other.object_);
        }

        return *this;
    }

    BWRef & operator=(BWRef && other) {
        if (this != &other) {
            TRACK::UnregInstnc(Ptr(), this);
            TRACK::UnregInstnc(other.Ptr(), &other);
            object_ = std::move(other.object_);
            other.object_=nullptr;

            TRACK::RegInstnc(Ptr(), this);
        }

        return *this;
    }

    BWRef & operator=(WObject * in_value) {
        if (object_ != in_value) {
            Set(in_value);
        }

        return *this;
    }

    // BWRef & operator=(const WObject & other) = delete;
    // BWRef & operator=(WObject && other) = delete;
    
    void Set(WObject * in_address) {
        TRACK::UnregInstnc(Ptr(), this);
        object_ = in_address;
        TRACK::RegInstnc(Ptr(), this);
    }

    WObject & Get() {
        return *object_;
    }

    WObject * Ptr() {
        return object_;
    }

    bool operator==(const BWRef & other) const
    {
        return object_ == other.object_;
    }

    bool operator!=(const BWRef & other) const
    {
        return object_ != other.object_;
    }


    bool operator==(WObject * other) const
    {
        return object_ == other;
    }

    bool operator!=(WObject * other) const
    {
        return object_ != other;
    }

    operator bool() const { return IsValid(); }

    constexpr bool IsValid() const noexcept { return object_ != nullptr; }

    constexpr bool IsEmpty() const noexcept { return object_ == nullptr; }

    static const std::vector<BWRef*> & Instances(WObject * in_ptr) {
        return TRACK::Instances(in_ptr);
    }

private:

    WObject * object_;

};

template<std::derived_from<WObject> T>
class TWRef : BWRef {
private:

    using TRACK = TWRefTrack_<TWRef<WObject>>;
    
public:

    constexpr TWRef()=default;

    TWRef(T * in_object)=default;

    TWRef(T & in_object) :
        BWRef(&in_object) {}

    TWRef(const TWRef & in_other) = default;

    TWRef(TWRef && in_other) = default;

    

    TWRef & operator=(const T & other) = delete;
    TWRef & operator=(T && other) = delete;

    T * operator->() 
    {
        return static_cast<T*>(Ptr());
    }

    const T * operator->() const
    {
        return static_cast<T*>(Ptr());
    }

    T & operator*()
    {
        return *static_cast<T*>(Ptr());
    }

    const T & operator*() const
    {
        return *static_cast<T*>(Ptr());
    }

};

