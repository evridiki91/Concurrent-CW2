#include "hilevel.h"


#define TOTALP 30
#define PIPES 32

pcb_t  pcb[TOTALP], *current = NULL;
pipe_t pipe[PIPES];

uint16_t fb[ 600 ][ 800 ];
int nbytes;
uint16_t cursorX; //Cursor's xcoordinates
uint16_t cursorY; //Cursor's ycoordinates


/*
 * User programs:
 */
extern void     main_P3();
extern uint32_t tos_P3;
extern void     main_P4();
extern uint32_t tos_P4;
extern void     main_P5();
extern uint32_t tos_P5;


extern void     main_console();
extern uint32_t tos_console;

startOfStack = &tos_console; //Make  the start of the stack the console's tos.


/*newPid() function : returns the next available
* id by going through a loop of all the processes and
* checking which id is not in use by another process.
*/
pid_t newPid( ){
  for( int i = 0; i < TOTALP; i++ ){
    if( pcb[ i ].available == 1){
      pcb[ i ].available = 0;   //make it unavailable
      return i;
    }
  }
  return -1;
}

/*nextProcess() function: is used in order to offer a round
* robin scheduler. It returns the next process that needs to be
* proceed, in a cycle order.
*/
pid_t nextProcess( ){
  for( int i = 0; i < TOTALP; i++ ){
    if( pcb[( ( i + ( current->pid ) + 1) % TOTALP ) ].available == 0){
       return ( i + ( current->pid ) + 1 ) % TOTALP;
    }
  }
   print("Error: there is not any process to be executed. \n");
}

/*maxPriority() function: is used in order to find the next process
* that is going to be executed based on their priority and age.
*/
pid_t maxPriority( ){
  int max = -1;
  pid_t nextId;
  for( int i = 0; i < TOTALP; i++ ){
    if ( ( pcb[ i ].priority ) > max ){
      max     = pcb[ i ].priority;
      nextId  = i;
    }
    else if ( ( pcb[ i ].priority ) == max ){
        if ( ( pcb[ i ].age ) > ( pcb[ nextId ].age ) ){
          max    = pcb[ i ].priority;
          nextId = i;
        }
      }
    }
  return nextId;
}

/* Priority scheduler: it uses the maxPriority() function.
 *Solves the starvation problemby by introducing aging. Each round
 *every process's age and priority is increased, except the current's.
*/
/*
void scheduler( ctx_t* ctx ) {
    pid_t new = maxPriority( );
    memcpy( &pcb[ current->pid ].ctx, ctx, sizeof( ctx_t ) );
    memcpy( ctx, &pcb[ new ].ctx, sizeof( ctx_t ) );
    current  = &pcb[ new ];
    for ( int i = 0; i < TOTALP; i++ ){
      if( i != ( current->pid ) && ( pcb[ i ].available == 0)){ //Solves starvation
        pcb[i].priority ++;
        pcb[i].age ++;
      }
    }
  return;
}
*/


/* Round Robin scheduler: uses the nextProcess() function.
*/

void scheduler( ctx_t* ctx ) {
    pid_t new = nextProcess();
    memcpy( &pcb[ current->pid ].ctx, ctx, sizeof( ctx_t ) );
    memcpy( ctx, &pcb[ new ].ctx, sizeof( ctx_t ) );
    current = &pcb[ new ];
  return;
}

/*******************************************************************************
FUNCTIONS USED FOR INTER PROCESS COMMUNICATION (IPC) ***************************
********************************************************************************

/* initialisePipes( ) function: Initialises all the pipes
*/
void initialisePipes( ) {
for( int i = 0; i < PIPES; i++ ){
  memset( &pipe[ i ], -1, sizeof( pipe_t ) );
  }
}


/* nextPipe() function: returns the index of a pipe if that pipe is
*available.(if it doesn't belong to someone).
*/
int nextPipe( int sender, int receiver ){
  int index = -1;
  for( int i = 0; i < PIPES; i++ ){
    if( ( pipe[ i ].sender == -1) && ( pipe[ i ].receiver == -1) ) {
      index = i;
      break;
    }
  }

    if( index != -1 ){
    pipe[ index ].sender    = sender;
    pipe[ index ].receiver  = receiver;
    pipe[ index ].full      = 0;
    }

return index;
}

/*******************************************************************************
FUNCTIONS USED FOR MANAGEMENT OF THE LCD AND PS/2 CONTROLLERS*******************
********************************************************************************
*/

/* ctoasc (char c) function: is used to convert a character c
* to its corresponding ascii code.
*/
int ctoasc( char c){
  int x;
  x = c;
  return x;
}

/* printpixels function: is used to print pixelated letters on the LCD by using
* a lookupTable first to find the corresponding character that came from the
* keyboard interrupt, and then another table to find the letter's format.
*/
void printpixels( int asc, int x, int y ){
  int val;
  for( int i = 16; i >= 0; i-- ){
    int pix = ascii[ asc ][ i ];
    for( int j = 0; j < 16; j++ ){
      val = ( pix >> j ) & 0x1; // pixel is shifted to see whether the bit is 1 or 0.
      if (val == 1) fb[ y + i ][ x + j ] = 0;
      else fb[ y + i ][ x + j ] = 0x7FFF;
    }
  }
}


/* resetImage() function: is used to set all the pixels of the LCD to black.
*/
void resetImage( ){
  for( int i = 0; i < 600; i++ ){
    for( int j = 0; j < 800; j++ ){
      fb[ i ][ j ] = 0;
    }
  }
}


/* mouseCursor function: sets all the pixels that are needed to create a cursor
* image on the screen.
*/
void mouseCursor( int x, int y ){
for( int i = 0; i < 16; i++ ){
  for( int j = 0; j < 16; j++ ){
    int val = ( cursor[ i ] >> j ) & 0x1;
    if ( val == 1 ) fb[ y + i ][ x + j ] = 0x7FFF;
    }
  }
}


/*clickCursor function: sets all the pixels that are needed to create a click_cursor
* image on the screen.
*/
void clickCursor( int x, int y ){
for( int i = 0; i < 16; i++ ){
  for( int j = 0; j < 16; j++ ){
    int val = ( cursor_click[ i ] >> j ) & 0x1;
    if ( val == 1 ) fb[ y + i ][ x + j ] = 0x7FFF;
    }
  }
}

/*Clears the current cursor from LCD.
*/
void clearCursor( ){
  for( int i = 0; i < 16; i++ ){
    for( int j = 0; j < 16; j++ ){
      fb[ cursorY + i ][ cursorX + j ] = 0;
    }
  }
}

//Just a function to clear bits
uint8_t clear_bit(uint8_t x, int bit){
  return (x &= ~(1 << bit));
}

int offsetLettersX = 0, offsetLettersY = 0;


/*******RESET*******
*******************/

void hilevel_handler_rst( ctx_t* ctx) {

  // Configure the LCD display into 800x600 SVGA @ 36MHz resolution.

  SYSCONF->CLCD      = 0x2CAC;     // per per Table 4.3 of datasheet
  LCD->LCDTiming0    = 0x1313A4C4; // per per Table 4.3 of datasheet
  LCD->LCDTiming1    = 0x0505F657; // per per Table 4.3 of datasheet
  LCD->LCDTiming2    = 0x071F1800; // per per Table 4.3 of datasheet

  LCD->LCDUPBASE     = ( uint32_t )( &fb );

  LCD->LCDControl    = 0x00000020; // select TFT   display type
  LCD->LCDControl   |= 0x00000008; // select 16BPP display mode
  LCD->LCDControl   |= 0x00000800; // power-on LCD controller
  LCD->LCDControl   |= 0x00000001; // enable   LCD controller

  /* Configure the mechanism for interrupt handling by
   *
   * - configuring then enabling PS/2 controllers st. an interrupt is
   *   raised every time a byte is subsequently received,
   * - configuring GIC st. the selected interrupts are forwarded to the
   *   processor via the IRQ interrupt signal, then
   * - enabling IRQ interrupts.
   */

  PS20->CR           = 0x00000010; // enable PS/2    (Rx) interrupt
  PS20->CR          |= 0x00000004; // enable PS/2 (Tx+Rx)
  PS21->CR           = 0x00000010; // enable PS/2    (Rx) interrupt
  PS21->CR          |= 0x00000004; // enable PS/2 (Tx+Rx)

  uint8_t ack;

        PL050_putc( PS20, 0xF4 );  // transmit PS/2 enable command
  ack = PL050_getc( PS20       );  // receive  PS/2 acknowledgement
        PL050_putc( PS21, 0xF4 );  // transmit PS/2 enable command
  ack = PL050_getc( PS21       );  // receive  PS/2 acknowledgement


  GICD0->ISENABLER1 |= 0x00300000; // enable PS2          interrupts


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

  resetImage();
  int nbytes = 0; // Initialise the counter of bytes for mouse interrupt
  cursorX = 100; //X-coordinate
  cursorY = 400; //Y -coordinate


  /* Initialise PCBs representing processes stemming from execution of
     * the two user programs.  Note in each case that
     *
     * - the CPSR value of 0x50 means the processor is switched into USR
     *   mode, with IRQ interrupts enabled, and
     * - the PC and SP values matche the entry point and top of stack.
     */

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

/******************************************************************************
*********INTERRUPT REQUEST HANDLER*********************************************
*******************************************************************************
*/
void hilevel_handler_irq(ctx_t* ctx) {

  // Step 2: read  the interrupt identifier so we know the source.
  uint32_t id = GICC0->IAR;

  // Step 4: handle the interrupt, then clear (or reset) the source.
  if( id == GIC_SOURCE_TIMER0 ) {
    scheduler(ctx);

    TIMER0->Timer1IntClr = 0x01;
  }

  // Step 4: handle the interrupt, then clear (or reset) the source.
  //PS20 - KEYBOARD
  else if     ( id == GIC_SOURCE_PS20 ) {
    uint8_t x = PL050_getc( PS20 );

    /*PL011_putc( UART0, '0',                    true );
    PL011_putc( UART0, '<',                      true );
    PL011_putc( UART0, itox( ( x >> 4 ) & 0xF ), true );
    PL011_putc( UART0, itox( ( x >> 0 ) & 0xF ), true );
    PL011_putc( UART0, '>',                      true );
    */
    if ((x >> 7) == 0 ) {
      PL011_putc( UART0, lookup[x], true ); //Shows what is pressed on the keyboard
      int asc = ctoasc(lookup[x]);
      print(" is pressed \n");
      printpixels(asc, 50, 50);
    }

      else {
        uint8_t newx = clear_bit(x, 7);
        PL011_putc(UART0, lookup[newx] ,true);
        print(" is released \n");
        resetImage();
    }
  }

  //MOUSE INTERRUPT
  else if( id == GIC_SOURCE_PS21 ) {

    clearCursor();
    uint16_t byte1;
    uint16_t byte2;
    uint16_t byte3;

    uint8_t x = PL050_getc( PS21 );
/*
    PL011_putc( UART0, '1',                      true );
    PL011_putc( UART0, '<',                      true );
    PL011_putc( UART0, itox( ( x >> 4 ) & 0xF ), true );
    PL011_putc( UART0, itox( ( x >> 0 ) & 0xF ), true );
    PL011_putc( UART0, '>',                      true );
    */
    switch (nbytes) {

     case 0 : {
       uint16_t byte1      = ( uint16_t )( x );
       if( ( (byte1 >> 0) & 0x1) != 0){
         print("Left button is pressed \n");
         clearCursor();
         clickCursor(cursorX, cursorY);
       }
    // uint16_t y_sign_bit = (byte1 >> 5) & 0x1;
    // uint16_t x_sign_bit = (byte1 >> 4) & 0x1;
       nbytes              = (nbytes + 1) % 3;

       break;
      }

     case 1 : {
       uint16_t byte2      = ( uint16_t )( x );
       int16_t x_movement  = byte2 - ((byte1 << 4) & 0x100);
       x_movement          = x_movement/16;
       print("x must move: "); print_int(x_movement); print("\n");
       if((cursorX + x_movement) <= 0) cursorX = 0;
       else if(cursorX >=800) cursorX = 800;
       else cursorX       = (cursorX + x_movement );
       nbytes              = (nbytes + 1) % 3;

       break;
      }

     case 2 : {
       uint16_t byte3      = ( uint16_t )( x );
       int16_t y_movement  = byte3 - ((byte1 << 3) & 0x100);
       y_movement          = y_movement/16;
       print("y must move: "); print_int(y_movement); print("\n");

       if((cursorY - y_movement) <= 0) cursorY = 0;
       else if(cursorY >=600) cursorY = 600;
       else cursorY       = (cursorY - y_movement );
       mouseCursor( cursorX, cursorY );
       print("X = "); print_int(cursorX); print(" and ");
       print("Y = "); print_int(cursorY); print("\n");

       nbytes              = (nbytes + 1) % 3;
       break;
      }
    }

}


  // Step 5: write the interrupt identifier to signal we're done.
  GICC0->EOIR = id;

  return;
}

/******************************************************************************
***************SVC HANDLER*****************************************************
******************************************************************************/


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

        if(newId != -1){
          memset( &pcb[ newId ], 0, sizeof( pcb_t ) );

          memcpy( &pcb[ newId ].ctx, ctx, sizeof( ctx_t ) ); //memcpy(destination,context,size)
          offset                 = ( pcb[ current->pid ].tos ) - ( ctx->sp ) ;
          pcb[ newId ].tos       = startOfStack + ( newId * 0x00001000 ); //finds the new top of the stack for newId
          pcb[ newId ].ctx.sp    = pcb[ newId ].tos - offset;

          pcb[ newId ].pid       = newId;  //The pid of the new process will be the newId
          pcb[ newId ].available = 0;
          pcb[ newId ].priority  = pcb[ current->pid ].priority; //Same priority as the parent
          pcb[ newId ].age       = 0;  //Set the child's age to 0;

          ctx->gpr[ 0 ] = newId; //Returns the child's id to the parent
          pcb[ newId ].ctx.gpr[ 0 ] = 0;  //fork returns zero to child's gpr[0] (output).

      }
        else {
          print( "Fork failed \n");
          ctx->gpr[ 0 ] = -1;
        }

        break;
      }

      case 0x04 : { // 0x04 => exit( int x )
        int    x  =  ( int  )( ctx->gpr[ 0 ] );
        pcb[ current->pid ].available = 1;
        pcb[ current->pid ].age       = 0;
        pcb[ current->pid ].priority  = 0;
        scheduler(ctx);
        break;
      }

      case 0x05 : { // 0x05 =>  exec( const void* x )
        void*  x   = ( void*    )( ctx->gpr[ 0 ] );
        ctx->pc    = ( uint32_t )( x ); //The program counter must be updated to the address x.
        ctx->sp    = pcb[ current->pid ].tos; //we don't want to use the stuff in the sp so we set the sp to the tos
        ctx->cpsr  = 0x50;
        break;
      }

      case 0x06 : { //0x06 => int kill( int pid, int x )
        int pid    = ( int )( ctx->gpr[ 0 ] );
        int x      = ( int )( ctx->gpr[ 1 ] );
        pcb[ pid ].available = 1;
        pcb[ pid ].age = 0;
        pcb[ pid ].priority = 0;
        pcb[ pid ].ctx.gpr[ 0 ] = x;
        scheduler( ctx ); //TODO: if i remove this will the timer change the process to the next one
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
        int id              = ( int )( ctx->gpr[ 0 ] );
        enum request_t ans  = pipe[id].data;
        ctx->gpr[ 0 ]       = ans;
        break;
      }

      case 0x09 : { //0x09 => writec( int id, request_t request)
        int id                 = ( int )( ctx->gpr[ 0 ] );
        enum request_t request = ( enum request_t )( ctx->gpr[ 1 ] );
        pipe[ id ].data        = request;
        break;
      }

      case 0xA  : { //0xA => getpid( );
        int id        = current->pid - 2;  //Finds the current philosopher process
        ctx->gpr[ 0 ] = id;
        break;
      }

      case 0xB  : { //0xB => isfull(int id);
        int id        = ( int )( ctx->gpr[0] );
        int ans       = pipe[id].full;
        ctx->gpr[ 0 ] = ans;
        break;
      }

      case 0xC  : { //0xC => changec(int id, int x);
        int id        = ( int )( ctx->gpr[0] );
        int x         = ( int )( ctx->gpr[1] );
        pipe[id].full = x;
        break;
      }

      case 0xD : { //0xD => showphil(int x, int y, int id);
        int x         = ( int )( ctx->gpr[0] );
        int y         = ( int )( ctx->gpr[1] );
        int id        = ( int )( ctx->gpr[2] );


        for(int letter = 0; letter<16; letter++) {
          char c = phileat[id][letter];
          for( int i = 16; i >= 0; i-- ){
            for( int j = 0; j < 16; j++ ){
              //print('c');
              int val = ctoasc(c);
              int pix = ascii[val][i];
              if (((pix >> j) & 0x01) == 0) fb[y + i + offsetLettersY*16 ][x + j + offsetLettersX*16] = 0x7FFF;
              else fb[y + i +  offsetLettersY*16][x + j + offsetLettersX*16] = 0;
            }

          }
          offsetLettersX++;

        }
          offsetLettersY ++;
          offsetLettersX = 0;
          break;
        }


      default   : { // 0x?? => unknown/unsupported
        break;
      }
    }

  return;
}
