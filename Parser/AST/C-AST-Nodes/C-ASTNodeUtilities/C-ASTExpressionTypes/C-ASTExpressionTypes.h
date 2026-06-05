#pragma once
#include <stdint.h>

#define MAX_PARAMS 128
#define MAX_CASES  1024

/* Forward-declare every node so the wrapper headers and sub-headers
   can reference any of them as pointer types before full definitions. */
typedef struct CBlockItem    CBlockItem;
typedef struct CFactor       CFactor;
typedef struct CStatement    CStatement;
typedef struct CDeclaration  CDeclaration;
typedef struct CBlock        CBlock;
typedef struct CCompound     CCompound;
typedef struct CBinary       CBinary;
typedef struct CUnary        CUnary;

/* External dynamic-array wrappers (depend on the forward decls above) */
#include "../../../../../DataStructures/DynamicArray/Wrappers/BlockItemArrayWrapper.h"
#include "../../../../../DataStructures/DynamicArray/Wrappers/IdentifierWrapper.h"
#include "../../../../../DataStructures/DynamicArray/Wrappers/ExpressionFactorWrapper.h"
#include "../../../../../DataStructures/DynamicArray/Wrappers/CInitializerListWrapper.h"

/* Shared operator / type enums from the parser common layer */
#include "../../../../Common/SharedNodeEnums.h"

typedef BlockItemArray CBlockItemList;

/* ── Sub-headers in dependency order ─────────────────────────────────
   Specifiers  <-  Expressions  <-  Statements  <-  Declarations      */
#include "Specifiers.h"
#include "Expressions.h"
#include "Statements.h"
#include "Declarations.h"
