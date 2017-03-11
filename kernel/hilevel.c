#include "hilevel.h"

#define TOTALP 15
pcb_t pcb[15], *current = NULL;


//From W3
extern void     main_P3();
extern uint32_t tos_P3;
extern void     main_P4();
extern uint32_t tos_P4;
extern void     main_P5();
extern uint32_t tos_P5;
extern void     main_console();
extern uint32_t tos_console;

startOfStack = &tos_P3;


pid_t newPid( ){
  for(int i = 0; i < TOTALP; i++ ){
    if (pcb[i].available == 1){
      pcb[i].available = 0;    //make it unavailable
      return i;
    }

  }
  return -1;
}

pid_t nextAvailable( ){
  pid_t next;
  for(int i = 0; i < TOTALP; i++ ){
    if (pcb[( (i + (current->pid) + 1) % TOTALP)].available == 0){
      next = (i + (current->pid) + 1) % TOTALP;
      return next;
    }
  }
  //return error
}

//Added from worksheet3
//TODO : giati en toso argo, pezi rolo to scheduler ?
void scheduler( ctx_t* ctx ) {
    pid_t new = nextAvailable();
    memcpy( &pcb[current->pid].ctx, ctx, sizeof( ctx_t ) );
    memcpy( ctx, &pcb[new].ctx, sizeof( ctx_t ) );
    current = &pcb[new];

  return;
}




//From W3
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
    pcb[ 0 ].ctx.pc    = ( uint32_t )( &main_P3 );
    pcb[ 0 ].ctx.sp    = ( uint32_t )( &tos_P3  );
    pcb[ 0 ].available = 0;
    pcb[ 0 ].tos   = ( uint32_t )( &tos_P3  );


    memset( &pcb[ 1 ], 0, sizeof( pcb_t ) );
    pcb[ 1 ].pid      = 1;
    pcb[ 1 ].ctx.cpsr = 0x50;
    pcb[ 1 ].ctx.pc   = ( uint32_t )( &main_P4 );
    pcb[ 1 ].ctx.sp   = ( uint32_t )( &tos_P4 );
    pcb[ 1 ].available = 0;
    pcb[ 1 ].tos   = ( uint32_t )( &tos_P4  );

    memset( &pcb[ 2 ], 0, sizeof( pcb_t ) );
    pcb[ 2 ].pid      = 2;
    pcb[ 2 ].ctx.cpsr = 0x50;
    pcb[ 2 ].ctx.pc   = ( uint32_t )( &main_P5 );
    pcb[ 2 ].ctx.sp   = ( uint32_t )( &tos_P5  );
    pcb[ 2 ].available = 0;
    pcb[ 2 ].tos   = ( uint32_t )( &tos_P5  );

    memset( &pcb[ 3 ], 0, sizeof( pcb_t ) );
    pcb[ 3 ].pid      = 3;
    pcb[ 3 ].ctx.cpsr = 0x50;
    pcb[ 3 ].ctx.pc   = ( uint32_t )( &main_console );
    pcb[ 3 ].ctx.sp   = ( uint32_t )( &tos_console  );
    pcb[ 3 ].available = 0;
    pcb[ 3 ].tos   = ( uint32_t )( &tos_console  );

current = &pcb[ 0 ]; memcpy( ctx, &current->ctx, sizeof( ctx_t ) );

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

        if(newId != -1){ //If fork has succesfully returned a child
          //memset?

          uint32_t newId_sp, newId_tos ;

          memcpy( &pcb[newId], current->ctx, sizeof( ctx_t ) ); //memcpy(destination,context,size)
          offset    = (ctx->sp) - (pcb[current->pid].tos);
          newId_tos = startOfStack + newId * 0x00001000;  //find the new top of the stack for newId
          newId_sp  = pcb[newId].tos + offset;

          memcpy( newId_tos, pcb[current->pid].tos, offset);       //Location on the stack for the child same size as the parent
          pcb[newId].pid = newId;  //The pid of the new process will be the newId

      }
      //else //return error message if the fork wasn't successful

        // new process with new pcb
        //
        ctx->gpr[ 0 ] = newId; //Returns the child's id to the parent
        pcb[newId].ctx.gpr[0] = 0;  //fork returns zero to child's gpr[0] (output).
        break;
      }

      case 0x04 : { // 0x04 => exit( int x )
        int    x  =  ( int  )( ctx->gpr[ 0 ] );
        pcb[current->pid].available = 1;
        scheduler(ctx);
      }

      case 0x05 : { // 0x05 =>  exec( const void* x )
        char*  x  = ( char* )( ctx->gpr[ 0 ] );
        ctx->pc    = x; //The program counter must be updated to the address x.
      }

      default   : { // 0x?? => unknown/unsupported
        break;
      }
    }

  return;
}
