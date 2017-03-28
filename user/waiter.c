#include "waiter.h"
#define PHILS 5


extern void main_philosophers();


typedef struct{
  int forks[PHILS];
  int wtp[PHILS];
  int ptw[PHILS];
} waiter_t;

void thinking1(){
  for(int i = 0; i < 0x20000000; i++)
    asm volatile( "nop" );
}

void main_waiter(){
  waiter_t waiter;

  print("This is waiter \n");
  for(int i = 0; i < PHILS; i++){
    waiter.wtp[i]   = cpipe(PHILS, i);
    //print("wtp "); print_int(i); print(" is no"); print_int(waiter.wtp[i]); print("\n");
    waiter.ptw[i]   = cpipe(i, PHILS);
  //  print("ptw "); print_int(i); print(" is no"); print_int(waiter.ptw[i]); print("\n");
    waiter.forks[i] = 1;
  }

  for(int i = 0; i < PHILS; i++){
    pid_t pid = fork();
  //  print("Forking philosopher \n"); //When fork is called the child process will continue from here (pc is saved)

      if(pid == 0) {
        //print("Going to execute philosopher");
        exec( &main_philosophers);
      }
    }

  while(1){ //Wait until someone makes a request
    for(int i = 0; i < PHILS; i++){
      enum request_t data = readc(waiter.ptw[i]);
      //print(" Waiter "); print(" receives request"); print_int(data); print("from ");print_int(i);print("\n");
      //print(" Reads request from pipe "); print_int(waiter.ptw[i]); print(" \n \n");

      if(data == eat) {

    // Check to see if there are available forks for phil
        if((waiter.forks[i] == 1) && (waiter.forks[(i + 1) % PHILS] == 1)) {
        //  print("Left fork for philosopher no: "); print_int(i); print("is: "); print_int(waiter.forks[i]);
        //  print("Right fork for philosopher no: "); print_int(i); print("is: "); print_int(waiter.forks[(i + 1) % PHILS]);
          //print(" Check forks for phil "); print_int(i); print(" \n \n");
          writec(waiter.wtp[i], okay); //Inform the philosopher that it's okay to eat
          //print(" Waiter "); print(" sent "); print_int(readc(waiter.wtp[i])); print("\n");
          print(" Waiter permits phil "); print_int(i); print(" to eat \n");
          waiter.forks[i] = 0; //Make the forks unavailble
          waiter.forks[(i + 1) % PHILS] = 0;
    }
      else {
        writec(waiter.wtp[i],notokay); //Inform the philosopher that he can't eat
        //print(" Waiter "); print(" sent "); print_int(readc(waiter.wtp[i])); print("\n");
      //print(" Waiter denies phil no: "); print_int(i); print(" to eat \n ");
      }
    }

    else if(data == finish){
      waiter.forks[i] = 1; //Make the forks available again
      waiter.forks[(i + 1) % PHILS] = 1;
    }

    }
  }
//  print("continuing waiter after forks \ns");


  }
