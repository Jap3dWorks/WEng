#pragma once

#include <unordered_map>
#include <vector>
#include <concepts>

#include <cassert>

class WObject;

template<typename T>
struct TWRefTrack_ {

    static void UnregInstance(void * in_address, T * in_obj) {
        if (!in_obj) return;

        // assert(Track()[in_address].contains(in_obj));

        // TODO erase elements

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

    BWRef(WObject * in_object) :
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

    BWRef & operator=(WObject * in_value) {
        if (object_ != in_value) {
            BSet(in_value);
        }

        return *this;
    }

    BWRef & operator=(const WObject & other) = delete;
    BWRef & operator=(WObject && other) = delete;
    
    virtual void BSet(WObject * in_address) {
        TRACK::UnregInstance(object_, this);
        object_ = in_address;
        TRACK::RegInstance(object_, this);
    }

    WObject * BPtr() {
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

    static bool IsInstanced(WObject * in_ptr) {
        return TRACK::Contains(in_ptr);
    }

    static const std::vector<BWRef*> & Instances(WObject * in_ptr) {
        return TRACK::Instances(in_ptr);
    }

protected:    

private:

    WObject * object_;

};

template<std::derived_from<WObject> T>
class TWRef : public BWRef {

public:

    constexpr TWRef() noexcept = default;

    TWRef(const TWRef & in_other) = default;

    TWRef(TWRef && in_other) = default;

    ~TWRef() = default;

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

    TWRef & operator=(const T & other) = delete;
    TWRef & operator=(T && other) = delete;

    void BSet(WObject * in_object) override final {
        if (in_object == nullptr) {
            BWRef::BSet(in_object);
        }
        else {
            // Debugging assert only
            // Should do I add a runtime check?
            assert(dynamic_cast<T*>(in_object));

            BWRef::BSet(in_object);
        }
    }

    void Set(T * in_object) {
        BWRef::BSet(in_object);
    }

    constexpr T * Ptr() noexcept {
        return static_cast<T*>(BPtr());
    }

    T * operator->() 
    {
        return static_cast<T*>(BPtr());
    }

    const T * operator->() const
    {
        return static_cast<T*>(BPtr());
    }

    T & operator*()
    {
        return *static_cast<T*>(BPtr());
    }

    const T & operator*() const
    {
        return *static_cast<T*>(BPtr());
    }

};

