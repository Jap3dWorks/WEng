#ifndef _WCLASS_
#define _WCLASS_ WObject
#endif

#define IDENT(x) x
#define XSTR(x) #x
#define STR(x) XSTR(x)
#define PATH(_CLS) STR(IDENT(_CLS).WEngine.hpp)
#define _ICONCAT(_W,_CLS) _W##_CLS

#include PATH(_WCLASS_)

#include "WEngineObjects/WObject.hpp"
#include <memory>

#ifdef _CLASSNAME
#undef _CLASSNAME
#endif

#define _CLASSNAME(_CLS) _ICONCAT(WClass__,_CLS)

template<> std::unique_ptr<IObjectDataBase> _CLASSNAME(_WCLASS_)::CreateObjectDatabase() {
    TWAllocator<_WCLASS_> a;

    a.SetAllocateFn(
        []
        (_WCLASS_ *& prev, _WCLASS_* nptr, size_t n) {
            if (prev) {
                for(size_t i=0; i<n; i++) {
                    if (!BWRef::IsInstanced(prev + i)) {
                        continue;
                    }
                        
                    for (auto & ref : BWRef::Instances(prev + i)) {
                        if (ref == nullptr) continue;
                            
                        ref->BSet(nptr + i);
                    }
                }
            }
        });

    return std::make_unique<TObjectDataBase<_WCLASS_, TWAllocator<_WCLASS_>>>(a);

    return nullptr;
}

template<> const WObject * _CLASSNAME(_WCLASS_)::DefaultObject() const {
    return &_WCLASS_::GetDefaultObject();
}
