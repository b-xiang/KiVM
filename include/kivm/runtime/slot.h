//
// Created by kiva on 2018/3/21.
//
#pragma once

#include <shared/types.h>

namespace kivm {
    struct Slot {
        union {
            jint i32;
            jobject ref;
        };
    };
}
