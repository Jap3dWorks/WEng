#include "WObjectManager.h"

// Iterator
// --------
detail::WObjectContainerBase::Iterator::Iterator(WObject* object) : object_(object) {}


WObject& detail::WObjectContainerBase::Iterator::operator*()
{
    return *object_;
}

WObject* detail::WObjectContainerBase::Iterator::operator->()
{
    return object_;
}

detail::WObjectContainerBase::Iterator& detail::WObjectContainerBase::Iterator::operator++()
{
    ++object_;
    return *this;
}

detail::WObjectContainerBase::Iterator detail::WObjectContainerBase::Iterator::operator++(int)
{
    Iterator temp = Iterator(object_);
    object_++;
    return temp;
}

bool detail::WObjectContainerBase::Iterator::operator==(const Iterator& other)
{
    return object_ == other.object_;
}

bool detail::WObjectContainerBase::Iterator::operator!=(const Iterator& other)
{
    return object_ != other.object_;
}

// WObjectTypeContainerBase
// ------------------------

size_t detail::WObjectContainerBase::GetSize() const
{
    return size_;
}

void detail::WObjectContainerBase::AssignNewId(WObject* object)
{
    object->id_ = WId(size_++);
}

