#include "philosophers.h"

extern void main_waiter();

enum status_t {
  eating,
  thinking,
};


typedef struct {
  int id;
  enum status_t status;
  int ptw;
  int wtp;
} philosopher_t;

philosopher_t phil;

void eat_request(){
  writec(phil.id*2 + 1, 1);
}

void main_philosophers (){
  phil.id     = getpid(); //need a way to know which philosopher is being created. Use getpid system call
  phil.ptw    = phil.id * 2 + 1;
  phil.wtp    = phil.id * 2;
  phil.status = thinking;

  // Ask waiter permission to eat
  writec(phil.ptw,eat);
  enum request_t permission = readc(phil.ptw);



  }
