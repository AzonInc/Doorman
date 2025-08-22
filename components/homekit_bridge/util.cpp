#pragma once

#include "util.h"
#include <cstring>

namespace esphome {
    namespace homekit {
        
        char* strdup_psram(const char* s) {
            if (!s) return nullptr;
            size_t len = strlen(s) + 1;
            RAMAllocator<char> alloc;
            char* copy = alloc.allocate(len);
            if (copy) memcpy(copy, s, len);
            return copy;
        }
        
    }  // namespace homekit
}  // namespace esphome