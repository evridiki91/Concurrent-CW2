#include "hilevel.h"

#define TOTALP 30
#define PHILOSOPHERS 16
#define PIPES 32

pcb_t  pcb[TOTALP], *current = NULL;
pipe_t pipe[PIPES];

/*
extern void     main_P3();
extern uint32_t tos_P3;
extern void     main_P4();
extern uint32_t tos_P4;
extern void     main_P5();
extern uint32_t tos_P5;
*/
extern void     main_console();
extern uint32_t tos_console;

startOfStack = &tos_console;


pid_t newPid( ){
  for(int i = 0; i < TOTALP; i++ ){
    if (pcb[i].available == 1){
      pcb[i].available = 0;    //make it unavailable
      return i;
    }
  }
  return -1;
}

//next process to be run
pid_t nextProcess( ){
  for(int i = 0; i < TOTALP; i++ ){
    if (pcb[( (i + (current->pid) + 1) % TOTALP)].available == 0){
      return (i + (current->pid) + 1) % TOTALP;
    }
  }
  //return error
}

pid_t maxPriority(){
  int max = -1;
  pid_t nextId;
  for(int i = 0; i < TOTALP; i++){
    if ( (pcb[i].priority) > max){
      max = pcb[i].priority;
      nextId  = i;
    }
    else if ( (pcb[i].priority) == max){
        if ( (pcb[i].age) > (pcb[nextId].age) ){
          max = pcb[i].priority;
          nextId = i;
        }
      }
    }
  return nextId;
}


/*
void scheduler( ctx_t* ctx ) {
    pid_t new = maxPriority();
    memcpy( &pcb[current->pid].ctx, ctx, sizeof( ctx_t ) );
    memcpy( ctx, &pcb[new].ctx, sizeof( ctx_t ) );
    current = &pcb[new];
    for (int i = 0; i < TOTALP; i++){
      if(i != (current->pid) && (pcb[i].available == 0)){ //Solves starvation
        pcb[i].priority ++;
        pcb[i].age ++;
      }
    }
  return;
}
*/
void initialisePipes() {
for(int i = 0; i < PIPES; i++){
  memset( &pipe[ i ], -1, sizeof(pipe_t) );
  }
}


int nextPipe( int sender, int receiver ){
  int index = -1;
  for(int i = 0; i < PIPES; i++){
    if( (pipe[ i ].sender == -1) && (pipe[ i ].receiver == -1)) {
      index = i;
      break;
    }
  }

  if (index != -1){
  pipe[ index ].sender    = sender;
  pipe[ index ].receiver  = receiver;
  pipe[ index ].full      = 0;
  }

return index;

}



void scheduler( ctx_t* ctx ) {
    pid_t new = nextProcess();
    memcpy( &pcb[current->pid].ctx, ctx, sizeof( ctx_t ) );
    memcpy( ctx, &pcb[new].ctx, sizeof( ctx_t ) );
    current = &pcb[new];
  return;
}





void hilevel_handler_rst( ctx_t* ctx) {

  /* Configure the mechanism for interrupt handling by
   *
   * - configuring timer st. it raises a (periodic) interrupt for each
   *   timer tick,
   * - configuring GIC st. the selected interrupts are forwarded to the
   *   processor via the IRQ interrupt signal, then
   * - enabling IRQ interrupts.
   */

  TIMER0->Timer1Load  = 0x00100000; // select period = 2^20 ticks ~= 1 sec
  TIMER0->Timer1Ctrl  = 0x00000002; // select 32-bit   timer
  TIMER0->Timer1Ctrl |= 0x00000040; // select periodic timer
  TIMER0->Timer1Ctrl |= 0x00000020; // enable          timer interrupt
  TIMER0->Timer1Ctrl |= 0x00000080; // enable          timer

  GICC0->PMR          = 0x000000F0; // unmask all            interrupts
  GICD0->ISENABLER1  |= 0x00000010; // enable timer          interrupt
  GICC0->CTLR         = 0x00000001; // enable GIC interface
  GICD0->CTLR         = 0x00000001; // enable GIC distributor

  int_enable_irq();

  /* Initialise PCBs representing processes stemming from execution of
     * the two user programs.  Note in each case that
     *
     * - the CPSR value of 0x50 means the processor is switched into USR
     *   mode, with IRQ interrupts enabled, and
     * - the PC and SP values matche the entry point and top of stack.
     */
     //TODO: iS IT CORRECT
    for (int i = 0; i < TOTALP; i++){
      pcb[i].available = 1;
    }


    memset( &pcb[ 0 ], 0, sizeof( pcb_t ) );
    pcb[ 0 ].pid       = 0;
    pcb[ 0 ].ctx.cpsr  = 0x50;
    pcb[ 0 ].ctx.pc    = ( uint32_t )( &main_console );
    pcb[ 0 ].ctx.sp    = ( uint32_t )( &tos_console  );
    pcb[ 0 ].available = 0;
    pcb[ 0 ].tos       = ( uint32_t )( &tos_console  );
    pcb[ 0 ].priority  = 2;
    pcb[ 0 ].age       = 0;

/*
    memset( &pcb[ 1 ], 0, sizeof( pcb_t ) );
    pcb[ 1 ].pid       = 1;
    pcb[ 1 ].ctx.cpsr  = 0x50;
    pcb[ 1 ].ctx.pc    = ( uint32_t )( &main_P3 );
    pcb[ 1 ].ctx.sp    = ( uint32_t )( &tos_P3  );
    pcb[ 1 ].available = 0;
    pcb[ 1 ].tos       = ( uint32_t )( &tos_P3  );
    pcb[ 1 ].priority  = 20;
    pcb[ 1 ].age       = 0;



    memset( &pcb[ 2 ], 0, sizeof( pcb_t ) );
    pcb[ 2 ].pid       = 2;
    pcb[ 2 ].ctx.cpsr  = 0x50;
    pcb[ 2 ].ctx.pc    = ( uint32_t )( &main_P4 );
    pcb[ 2 ].ctx.sp    = ( uint32_t )( &tos_P4 );
    pcb[ 2 ].available = 0;
    pcb[ 2 ].tos       = ( uint32_t )( &tos_P4  );
    pcb[ 2 ].priority  = 3;
    pcb[ 2 ].age       = 0;



    memset( &pcb[ 3 ], 0, sizeof( pcb_t ) );
    pcb[ 3 ].pid       = 3;
    pcb[ 3 ].ctx.cpsr  = 0x50;
    pcb[ 3 ].ctx.pc    = ( uint32_t )( &main_P5 );
    pcb[ 3 ].ctx.sp    = ( uint32_t )( &tos_P5  );
    pcb[ 3 ].available = 0;
    pcb[ 3 ].tos       = ( uint32_t )( &tos_P5  );
    pcb[ 3 ].priority  = 1;
    pcb[ 3 ].age       = 0;
*/


current = &pcb[ 0 ]; memcpy( ctx, &current->ctx, sizeof( ctx_t ) );
initialisePipes();

  return;
}

void hilevel_handler_irq(ctx_t* ctx) {
  // Step 2: read  the interrupt identifier so we know the source.

  uint32_t id = GICC0->IAR;

  // Step 4: handle the interrupt, then clear (or reset) the source.

  if( id == GIC_SOURCE_TIMER0 ) {
    scheduler(ctx);

    TIMER0->Timer1IntClr = 0x01;
  }

  // Step 5: write the interrupt identifier to signal we're done.

  GICC0->EOIR = id;

  return;
}

void hilevel_handler_svc(ctx_t* ctx, uint32_t id) {
  /* Based on the identified encoded as an immediate operand in the
   * instruction,
   *
   * - read  the arguments from preserved usr mode registers,
   * - perform whatever is appropriate for this system call,
   * - write any return value back to preserved usr mode registers.
   */

  switch( id ) {

    /*  case 0x00 : { // 0x00 => yield()
        scheduler( ctx );
        break;
      }*/

      case 0x01 : { // 0x01 => write( fd, x, n )
        int   fd = ( int   )( ctx->gpr[ 0 ] );
        char*  x = ( char* )( ctx->gpr[ 1 ] );
        int    n = ( int   )( ctx->gpr[ 2 ] );

        for( int i = 0; i < n; i++ ) {
          PL011_putc( UART0, *x++, true );
        }

        ctx->gpr[ 0 ] = n;
        break;
      }

      case 0x03 : { // 0x03 => fork( )
        pid_t newId = newPid();
        uint32_t offset;
      //  PL011_putc( UART0, 'a', true );

        if(newId != -1){
          //memset?
          memset( &pcb[ newId ], 0, sizeof( pcb_t ) );

          memcpy( &pcb[newId].ctx, ctx, sizeof( ctx_t ) ); //memcpy(destination,context,size)
          offset    = (pcb[current->pid].tos) - (ctx->sp) ;
          pcb[newId].tos = startOfStack + (newId * 0x00001000);  //finds the new top of the stack for newId
          pcb[newId].ctx.sp  = pcb[newId].tos - offset;
          //PL011_putc( UART0,'b' , true );

          pcb[newId].pid = newId;  //The pid of the new process will be the newId
          pcb[newId].available = 0;
          pcb[newId].priority = pcb[current->pid].priority; //Same priority as the parent
          pcb[newId].age = 0;  //Set the child's age to 0;


          ctx->gpr[ 0 ] = newId; //Returns the child's id to the parent
          pcb[newId].ctx.gpr[0] = 0;  //fork returns zero to child's gpr[0] (output).
        //  PL011_putc( UART0, 'c', true );
      }
    //else write( STDOUT_FILENO, "Fork failed", 11);
//return error message if the fork wasn't successful

        // new process with new pcb
        //

        break;
      }

      case 0x04 : { // 0x04 => exit( int x )
        int    x  =  ( int  )( ctx->gpr[ 0 ] );
        pcb[current->pid].available = 1;
        pcb[current->pid].age = 0;
        pcb[current->pid].priority = 0;
        scheduler(ctx);
        break;
      }

      case 0x05 : { // 0x05 =>  exec( const void* x )
        void*  x   = ( void* )( ctx->gpr[ 0 ] );
        ctx->pc    = (uint32_t)(x); //The program counter must be updated to the address x.
        ctx->sp    = pcb[current->pid].tos; //we don't want to use the stuff in the sp so we set the sp to the tos
        ctx->cpsr  = 0x50;
      /*  for(int i = 0; i < 13; i++){
          ctx->gpr[i] = 0;
        }*/
        break;
      }

      case 0x06 : { //0x06 => int kill( int pid, int x )
        int pid    = ( int )( ctx->gpr[ 0 ] );
        int x      = ( int )( ctx->gpr[ 1 ] );
        pcb[pid].available = 1;
        pcb[pid].age = 0;
        pcb[pid].priority = 0;
        pcb[pid].ctx.gpr[0] = x;
        scheduler(ctx); //TODO: if i remove this will the timer change the process to the next one
        break;
      }

      case 0x07 : { //0x07 => cpipe(int start, int end )
        int start = ( int )( ctx->gpr[ 0 ] );
        int end   = ( int )( ctx->gpr[ 1 ] );
        int index = nextPipe( start, end );

        if(index != -1) ctx->gpr[ 0 ] = index;
        break;
      }

      case 0x08 : { //0x08 => readc( int id )
        int id   = ( int )( ctx->gpr[ 0 ] );
        enum request_t ans  = pipe[id].data;
        ctx->gpr[ 0 ] = ans;
        break;
      }

      case 0x09 : { //0x09 => writec( int id, request_t request)
        int id      = ( int )( ctx->gpr[ 0 ] );
        enum request_t request = ( enum request_t )( ctx->gpr[ 1 ] );
        pipe[id].data = request;
        break;
      }

      case 0xA  : { //0xA => getpid( );
        int id = current->pid - 2;  //Fins the current philosopher process
        ctx->gpr[ 0 ] = id;
        break;
      }

      case 0xB  : { //0xB => isfull(int id);
        int id = ( int )( ctx->gpr[0] );
        int ans = pipe[id].full;
        ctx->gpr[ 0 ] = ans;
        break;
      }

      case 0xC  : { //0xC => changec(int id, int x);
        int id = ( int )( ctx->gpr[0] );
        int x  = ( int )( ctx->gpr[1] );
        pipe[id].full = x;
        break;
      }

      default   : { // 0x?? => unknown/unsupported
        break;
      }
    }

  return;
}
