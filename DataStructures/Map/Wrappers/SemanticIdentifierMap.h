#pragma once
#include "../map.h"
#include "Parser/AST/C-AST-Nodes/C-ASTNodes.h"

typedef Map SemanticIdentifierMap;

SemanticIdentifierMap* createSemanticIdentifierMap();
SemanticIdentifierMap* copySemanticIdentifierMap(SemanticIdentifierMap* original);
void freeSemanticIdentifierMap(SemanticIdentifierMap* map);
int semanticMapPut(SemanticIdentifierMap* map, char* key, char* value, int isInScope, int hasLinkage);
char* semanticMapGet(SemanticIdentifierMap* map, char* key);
MapEntry* getSemanticMapEntry(SemanticIdentifierMap* map, char* key);
int semanticMapContainsKey(SemanticIdentifierMap* map, char* key);
int isFromCurrentBlock(SemanticIdentifierMap* map, char* key);
int semanticMapRemove(SemanticIdentifierMap* map, char* key);