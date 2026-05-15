#include "ScopeUtils.h"
#include <stdlib.h>

void markAllEntriesOutOfCurrentScope(SemanticIdentifierMap* varMap) {
    if (!varMap) return;

    for (size_t i = 0; i < varMap->bucket_count; i++) {
        MapEntry* current = varMap->buckets[i];
        while (current) {
            current->isInScope = 0;
            current = current->next;
        }
    }
}
