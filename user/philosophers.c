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

/*
void thinking(){
  for(int i = 0; i < 0x20000000; i++)
    asm volatile( "nop" );
}
*/

void main_philosophers ( ){
  philosopher_t phil;
  phil.id     = getpid( ); //need a way to know which philosopher is being created. Use getpid system call
  phil.ptw    = phil.id * 2 + 1;
  phil.wtp    = phil.id * 2;
  phil.status = hungry;



  while( 1 ){

if( phil.status == hungry ){
    if( isfull( phil.ptw ) == 0){ //If he hasn't make any requests yet
      writec ( phil.ptw, eat ); // Ask waiter permission to eat
      changec( phil.ptw, 1 ); //Make the channel full
    // print("Philosopher no: "); print_int(phil.id); print(" wants to eat. \n");
    //  thinking();
    }

    if( isfull( phil.wtp ) == 1){
      enum request_t permission = readc( phil.wtp );
      changec( phil.wtp, 0 );

      if( permission == okay ){

          print( "Philosopher no: " ); print_int( phil.id ); print(" is eating. \n");
          phil.status = nothungry; // The philosopher ate
        //  thinking();
          writec( phil.ptw,finish ); // Finished eating, inform the waiter
          changec( phil.ptw, 1 );
          print("Philosopher no: "); print_int( phil.id ); print(" has finished eating. \n");
        }
      }
    }
  }
}
