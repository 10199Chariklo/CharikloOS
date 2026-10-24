#ifndef CK_GRAPH_UTILS_H
#define CK_GRAPH_UTILS_H

/* Standard C11 Libraries */
#include <stdbool.h>

/* Chariklo subsystem */
#include "CKtypes.h"

/**
 * @brief Attaches a node handle as the last child of a parent node handle.
 */
void CKgraph_AppendChild(CKgraphNodeHandle parent, CKgraphNodeHandle child);

/**
 * @brief Sub-Tree Lifecycle Detachment Primitive.
 * Unlinks the node from its parent and adjacent siblings safely.
 */
void CKgraph_DetachSubTree(CKgraphNodeHandle node);

/**
 * @brief Type-agnostic visitor callback type matching the updated handle interface.
 */
typedef bool (*CKgraphVisitor)(CKgraphNodeHandle node, void* context);

/**
 * @brief Master execution driver for flattening passes and system layout traversal hooks.
 */
bool CKgraph_Flatten(CKgraphNodeHandle root, CKtraversalOrder order, CKgraphVisitor visitor, void* context);


#endif /* CK_GRAPH_UTILS_H */