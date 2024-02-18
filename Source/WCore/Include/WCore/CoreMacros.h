#pragma once
#include <memory>


#define WCLASS(...)


#define _WOBJECT_BODY_(_WCLASS_)                                        \
private:                                                                \
    static inline std::unique_ptr<_WCLASS_> default_object_{};          \
public:                                                                 \
    _WCLASS_() = default;                                               \
    ~_WCLASS_() = default;                                              \
    static const _WCLASS_* GetDefaultObject() {                         \
        if (!default_object_.get()) {                                   \
                default_object_ = std::make_unique<_WCLASS_>();         \
        }                                                               \
        return default_object_.get();                                   \
    }                                                                   \
    virtual WClass GetClass() const { return WClass( #_WCLASS_ ); }     \



#define WOBJECT_BODY(_WCLASS_)  \
_WOBJECT_BODY_(_WCLASS_)
