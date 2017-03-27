#include "philosophers.h"

extern void main_waiter();

enum status_t {
  hungry,
  nothungry,
};


typedef struct {
  int id;
  enum status_t status;
  int ptw;
  int wtp;
} philosopher_t;

void thinking(){
  for(int i = 0; i < 0x20000000; i++)
    asm volatile( "nop" );
}

void main_philosophers (){
  philosopher_t phil;
  phil.id     = getpid(); //need a way to know which philosopher is being created. Use getpid system call
  phil.ptw    = phil.id * 2 + 1;
  phil.wtp    = phil.id * 2;
  phil.status = hungry;


//  print("This is philosopher "); print_int(phil.id); print(" \n");
  while(1){
    //print("Inside while loop of philosopher "); print_int(phil.id); print(" \n");
if(phil.status == hungry){
  // Ask waiter permission to eat
  writec(phil.ptw,eat);
//  print(" Eat request to pwt pipe "); print_int(phil.ptw); print(" \n");
  print(" Philosopher "); print_int(phil.id); print(" sent request "); print_int(readc(phil.ptw)); print("\n");

  thinking();

  enum request_t permission = readc(phil.wtp);
  print(" philosopher ");print_int(phil.id); print(" receives permission"); print_int(permission); print("\n");

//  print(" Read permission from wtp pipe "); print_int(phil.wtp); print(" \n \n");

  if(permission == okay){
  print(" Philosopher "); print_int(phil.id); print(" is eating \n \n");

    phil.status = nothungry;    // The philosopher ate
    thinking();
    writec(phil.ptw,finish); // Finished eating, inform the waiter
    print(" Philosopher "); print_int(phil.id); print(" sent request "); print_int(readc(phil.ptw)); print("\n");
  //  print(" Finish to pwt pipe "); print_int(phil.ptw); print(" \n");
    print(" Philosopher "); print_int(phil.id); print(" has finished eating \n \n");
      }

    }
  }
}
