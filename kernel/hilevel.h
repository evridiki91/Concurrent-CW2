#ifndef __HILEVEL_H
#define __HILEVEL_H

// Include functionality relating to newlib (the standard C library).

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Include functionality relating to the platform.

#include   "GIC.h"
#include "PL011.h"
#include "SP804.h"

// Include functionality relating to the   kernel.

#include "lolevel.h"
#include "int.h"

//Added from worksheet3
typedef int pid_t;

typedef struct {
  uint32_t cpsr, pc, gpr[ 13 ], sp, lr;
} ctx_t;


enum request_t {
  okay,
  notokay,
  eat,
  finish,
};

typedef struct {
  pid_t pid;
  ctx_t ctx;
  int available;
  int priority;
  int age;
  uint32_t tos;
} pcb_t;

typedef struct {
  enum request_t data;
  pid_t sender;
  pid_t receiver;
} pipe_t;


#endif
