#pragma once
#include "../map.h"
#include "Parser/AST/C-AST-Nodes/C-ASTNodes.h"

typedef Map SemanticVariableMap;

SemanticVariableMap* createSemanticVariableMap();
SemanticVariableMap* copySemanticVariableMap(SemanticVariableMap* original);
void freeSemanticVariableMap(SemanticVariableMap* map);
int semanticMapPut(SemanticVariableMap* map, char* key, char* value);
char* semanticMapGet(SemanticVariableMap* map, char* key);
MapEntry* getSemanticMapEntry(SemanticVariableMap* map, char* key); 
int semanticMapContainsKey(SemanticVariableMap* map, char* key);
int isFromCurrentBlock(SemanticVariableMap* map, char* key);
int semanticMapRemove(SemanticVariableMap* map, char* key);