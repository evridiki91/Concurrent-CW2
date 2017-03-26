#include "waiter.h"

extern void main_philosophers();


typedef struct{
  int forks[16];
  int wtp[16];
  int ptw[16];
} waiter_t;

waiter_t waiter;


  void initialiseEverything(){
    for(int i = 0; i < 16; i++){
      waiter.wtp[i]   = cpipe(16, i);
      waiter.ptw[i]   = cpipe(i, 16);
      waiter.forks[i] = 1;
    }
  }

void main_waiter(){
  initialiseEverything();

  for(int i = 0; i < 16; i++){
    pid_t pid = fork();

      if(pid == 0) exec( &main_philosophers);


    }
  while(1){ //Wait until someone makes a request
    int pid;
    for(int i = 0; i < 16; i++){
      data = readc(i);
      if(data == eat) {
        pid = i;
        break;
      }
    }
    // Check to see if there are available forks for phil
    if((waiter.forks[pid] == 1) && (waiter.forks[(pid + 1) % 16] == 1)){
      writec(pid, okay); //TODO finish this
    }
  }


  }
