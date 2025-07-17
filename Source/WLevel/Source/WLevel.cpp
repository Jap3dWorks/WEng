#include "WLevel/WLevel.hpp"

void WLevel::ForEachActor(const WClass * in_class, TFunction<void(WActor*)> in_predicate) {
    for(auto& c : object_manager_.Classes()) {
        if(in_class == c || in_class->IsBaseOf(c)) {
            object_manager_.ForEach(in_class,
                                    [&in_predicate](WObject* _obj) {
                                        in_predicate(static_cast<WActor*>(_obj));
                                    }
                );
        }
    }
}
