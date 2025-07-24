#pragma once

#include <unordered_map>
#include <vector>
#include <cassert>

template<typename T>
struct TWRefTrack_ {

    static void UnregInstance(void * in_address, T * in_obj) {
        // TODO: use a better data structure
        
        if (!in_obj) return;

        assert(Track().contains(in_address));

        std::vector<T*> v;
        v.reserve(Track()[in_address].size());
        
        for (T* p : Track()[in_address]) {
            if (p != in_obj) {
                v.push_back(p);
            }
        }

        Track()[in_address] = std::move(v);
    }

    static void RegInstance(void * in_address, T * in_obj) {
        if (!in_obj) return;
        
        if (!Track().contains(in_address)) {
            Track()[in_address] = {};
            Track()[in_address].reserve(64);
        }

        Track()[in_address].push_back(in_obj);
    }

    static bool Contains(void * in_address) {
        return Track().contains(in_address);
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

    constexpr BWRef() noexcept :
        object_(nullptr) {}

    BWRef(void * in_object) :
        object_(in_object) {
        TRACK::RegInstance(object_, this);
    }

    BWRef(const BWRef & other) :
        object_(other.object_) {
        TRACK::RegInstance(object_, this);
    }

    BWRef(BWRef && other) :
        object_(std::move(other.object_)) {
        TRACK::UnregInstance(object_, &other);
        TRACK::RegInstance(object_, this);
    }

    virtual ~BWRef() {
        if (object_) {
            TRACK::UnregInstance(object_, this);
            object_ = nullptr;
        }
    }

    BWRef & operator=(const BWRef & other) {
        if (this != &other) {
            BSet(other.object_);
        }

        return *this;
    }

    BWRef & operator=(BWRef && other) {
        if (this != &other) {
            TRACK::UnregInstance(object_, this);
            TRACK::UnregInstance(other.object_, &other);
            object_ = std::move(other.object_);
            other.object_=nullptr;

            TRACK::RegInstance(object_, this);
        }

        return *this;
    }

    BWRef & operator=(void * in_value) {
        if (object_ != in_value) {
            BSet(in_value);
        }

        return *this;
    }

    virtual void BSet(void * in_address) {
        TRACK::UnregInstance(object_, this);
        object_ = in_address;
        TRACK::RegInstance(object_, this);
    }

    void * BPtr() {
        return object_;
    }

    const void * BPtr() const {
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

    bool operator==(void * other) const
    {
        return object_ == other;
    }

    bool operator!=(void * other) const
    {
        return object_ != other;
    }

    operator bool() const { return IsValid(); }

    constexpr bool IsValid() const noexcept { return object_ != nullptr; }

    constexpr bool IsEmpty() const noexcept { return object_ == nullptr; }

    static bool IsInstanced(void * in_ptr) {
        return TRACK::Contains(in_ptr);
    }

    static const std::vector<BWRef*> & Instances(void * in_ptr) {
        return TRACK::Instances(in_ptr);
    }

protected:    

private:

    void * object_;

};

template<typename T>
class TWRef : public BWRef {

public:

    constexpr TWRef() noexcept = default;

    TWRef(const TWRef & in_other) = default;

    TWRef(TWRef && in_other) = default;

    ~TWRef() override = default;

    TWRef(T * in_object) :
        BWRef(in_object) {}

    TWRef(T & in_object) :
        BWRef(&in_object) {}

    TWRef & operator=(T * in_value) {
        BWRef::operator=(in_value);
        return *this;
    }

    TWRef & operator=(T & in_value) {
        BWRef::operator=(&in_value);
        return *this;
    }

    TWRef & operator=(T && other) = delete;

    void Set(T * in_object) {
        BWRef::BSet(in_object);
    }

    constexpr T * Ptr() noexcept {
        return static_cast<T*>(BPtr());
    }

    constexpr const T * Ptr() const noexcept {
        return static_cast<const T*>(BPtr());
    }

    T * operator->() 
    {
        return static_cast<T*>(BPtr());
    }

    const T * operator->() const
    {
        return static_cast<const T*>(BPtr());
    }

    T & operator*()
    {
        return *static_cast<T*>(BPtr());
    }

    const T & operator*() const
    {
        return *static_cast<const T*>(BPtr());
    }

};

