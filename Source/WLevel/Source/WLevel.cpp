#include "WLevel/WLevel.hpp"

void WLevel::ForEachActor(const WClass * in_class, TFunction<void(WActor*)> in_predicate) {
    object_manager_.ForEach(in_class,
                            [&in_predicate](WObject* _obj) {
                                in_predicate(static_cast<WActor*>(_obj));
                            });
}
