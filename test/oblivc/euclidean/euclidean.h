#pragma once

#define ALLOC 128

typedef struct protocolIO {
  char *src; // Filename for data
  int n;     // Number of vector elements
  int dist;  // Distance
} protocolIO;

void euclideanDistance(void* args);
void load_data(protocolIO *io, int **x1, int **x2, int party);
