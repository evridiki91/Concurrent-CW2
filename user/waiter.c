#include "waiter.h"
#define PHILS 16

extern void main_philosophers();

typedef struct{
  int forks[PHILS];
  int wtp[PHILS];
  int ptw[PHILS];
} waiter_t;



void main_waiter(){
  waiter_t waiter;

  for( int i = 0; i < PHILS; i++ ){
    waiter.wtp[ i ]   = cpipe( PHILS, i );
    waiter.ptw[ i ]   = cpipe( i, PHILS );
    waiter.forks[ i ] = 1;
  }

  for( int i = 0; i < PHILS; i++ ){
    pid_t pid = fork();

      if( pid == 0 ) {
        //print("Going to execute philosopher");
        exec( &main_philosophers );
      }
    }
  print("\n");
    while(1){ //Wait until someone makes a request

      for( int i = 0; i < PHILS; i++ ){
        if( isfull(waiter.ptw[ i ]) == 1){
          enum request_t data = readc( waiter.ptw[ i ] );
          changec( waiter.ptw[ i ], 0 ); //Empty the pipe so the philospher can put other requests in the pipe

        if( ( data == eat ) ){
          // Check to see if there are available forks for phil
          if( ( (waiter.forks[ i ] ) == 1 ) && ( (waiter.forks[ (i + 1) % PHILS] ) == 1) )  {
            writec ( waiter.wtp[ i ], okay ); //Inform the philosopher that it's okay to eat
            changec( waiter.wtp[ i ], 1 );

            //print("\nPERMISSION\n");
            print(" Waiter permits phil: "); print_int(i); print(" to eat \n");
            waiter.forks[ i ] = 0; //Make the forks unavailble
            waiter.forks[ (i + 1) % PHILS ] = 0;
          }
          else {
            writec ( waiter.wtp[ i ],notokay ); //Inform the philosopher that he can't eat
            changec( waiter.wtp[ i ], 1);
            //print("\nDENY\n");
            print(" Waiter denies phil no: "); print_int(i); print(" to eat \n ");
          }
        }

      else if( data == finish ){

        waiter.forks[ i ] = 1; //Make the forks available again
        waiter.forks[ (i + 1) % PHILS ] = 1;
        }
      }
    }
  }
}
