#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <obliv.h>

#include "dbg.h"
#include "euclidean.h"
#include "../common/util.h"


void check_mem(int *x1, int *x2, int party) {
  if (party == 1) {
    if (x1 == NULL) {
      log_err("Memory allocation failed\n");
      clean_errno();
      exit(1);
    }
  }
  if (party == 2) {
    if (x2 == NULL) {
      log_err("Memory allocation failed\n");
      clean_errno();
      exit(1);
    }
  }
}


int main(int argc, char *argv[]) {
  ProtocolDesc pd;
  protocolIO io;

  printf("Euclidean Distance Calculation\n");
  printf("——————————————————————————————\n\n");

  // Check arguments
  if (argc == 4) {
    // Initialize protocols and obtain connection information
    const char *remote_host = strtok(argv[1], ":");
    const char *port = strtok(NULL, ":");

    // Make connection between two shells
    // Modified ocTestUtilTcpOrDie() function from test/oblivc/common/util.c
    log_info("Connecting to %s on port %s ...\n", remote_host, port);
    if (argv[2][0] == '1') {
      if (protocolAcceptTcp2P(&pd, port) != 0) {
        log_err("TCP accept from %s failed\n", remote_host);
        exit(1);
      }
    } else {
      if(protocolConnectTcp2P(&pd, remote_host, port) != 0) {
        log_err("TCP connect to %s failed\n", remote_host);
        exit(1);
      }
    }

    // Final initializations before entering protocol
    int current_party = atoi(argv[2]);
    if (current_party != 1 && current_party != 2) {
      log_err("Party number must be either 1 or 2\n");
      exit(1);
    }
    setCurrentParty(&pd, current_party);
    io.src = argv[3];
    double lap = wallClock();

    // Execute Yao's protocol and cleanup
    execYaoProtocol(&pd, euclideanDistance, &io);
    cleanupProtocol(&pd);
    // Stop the clock
    double runtime = wallClock() - lap;

    // Print results and gate count
    log_info("Total time: %lf seconds\n", runtime);
    // log_info("Yao Gate Count: %u\n", yaoGateCount());
    printf("\n");
    log_info("Squared distance \td = %i\n", io.dist);
  } else {
    log_info("Usage: %s <hostname:port> <1|2> <filename>\n"
             "\tHostname usage:\n"
             "\tlocal -> 'localhost' remote -> IP address or DNS name\n",
             argv[0]);
  }
  return 0;
  }


void load_data(protocolIO *io, int **x1, int **x2, int party) {
  FILE *inputFile = fopen(io->src, "r");
  if (inputFile == NULL) {
    log_err("File '%s' not found\n", io->src);
    clean_errno();
    exit(1);
  }

  io->n = 0;
  int memsize = ALLOC;
  int element;
  while (!feof(inputFile)) {
    // Read the integer from each line in the file
    int dataPoints = fscanf(inputFile, "%d", &element);
    if ( dataPoints != 1) {
      if (feof(inputFile))
        break;
      log_err("Input from '%s' does not match file format.\n", io->src);
      clean_errno();
      exit(1);
    }

    io->n += 1;
    if (io->n > memsize) {
      memsize *= 2;
      *x1 = realloc(*x1, sizeof(int) * memsize);
      *x2 = realloc(*x2, sizeof(int) * memsize);
      check_mem(*x1, *x2, party);
    }

    int adj = io->n - 1;
    if (party == 1)
      *(*x1 + adj) = element;
    else if (party == 2)
      *(*x2 + adj) = element;
  }

  log_info("Loaded %d data points\n", io->n);
  fclose(inputFile);
}
