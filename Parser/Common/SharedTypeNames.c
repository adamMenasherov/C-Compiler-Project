#include "SharedTypeNames.h"

const char* getStaticInitTypeName(initialValueStaticInitType type) {
    switch (type) {
        case STATIC_INIT_INT:
            return "INT";
        case STATIC_INIT_LONG:
            return "LONG";
        case STATIC_INIT_UNSIGNED_INT:
            return "UNSIGNED_INT";
        case STATIC_INIT_UNSIGNED_LONG:
            return "UNSIGNED_LONG";
        default:
            return "UNKNOWN";
    }
}
