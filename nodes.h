#ifndef _NODES_H
#define _NODES_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

const char *node_state(uint32_t state);
const char *node_state_short(uint32_t state);
void print_node_state(FILE *file, uint32_t state);
void print_nodes(FILE *file);

#ifdef __cplusplus
}
#endif

#endif /* _NODES_H */
