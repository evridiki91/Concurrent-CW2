#include "libc.h"

int  atoi( char* x        ) {
  char* p = x; bool s = false; int r = 0;

  if     ( *p == '-' ) {
    s =  true; p++;
  }
  else if( *p == '+' ) {
    s = false; p++;
  }

  for( int i = 0; *p != '\x00'; i++, p++ ) {
    r = s ? ( r * 10 ) - ( *p - '0' ) :
            ( r * 10 ) + ( *p - '0' ) ;
  }

  return r;
}

void itoa( char* r, int x ) {
  char* p = r; int t, n;

  if( x < 0 ) {
    p++; t = -x; n = 1;
  }
  else {
         t = +x; n = 1;
  }

  while( t >= n ) {
    p++; n *= 10;
  }

  *p-- = '\x00';

  do {
    *p-- = '0' + ( t % 10 ); t /= 10;
  } while( t );

  if( x < 0 ) {
    *p-- = '-';
  }

  return;
}

void print(char* message){

  while(*message != '\0'){
    write(STDOUT_FILENO, message, 1);
    message++;
  }
  return;
}

void yield() {
  asm volatile( "svc %0     \n" // make system call SYS_YIELD
              :
              : "I" (SYS_YIELD)
              : );

  return;
}

int write( int fd, const void* x, size_t n ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = fd
                "mov r1, %3 \n" // assign r1 =  x
                "mov r2, %4 \n" // assign r2 =  n
                "svc %1     \n" // make system call SYS_WRITE
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r) //output
              : "I" (SYS_WRITE), "r" (fd), "r" (x), "r" (n)
              : "r0", "r1", "r2" );

  return r;
}

int  read( int fd,       void* x, size_t n ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = fd
                "mov r1, %3 \n" // assign r1 =  x
                "mov r2, %4 \n" // assign r2 =  n
                "svc %1     \n" // make system call SYS_READ
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r)
              : "I" (SYS_READ),  "r" (fd), "r" (x), "r" (n)
              : "r0", "r1", "r2" );

  return r;
}

int fork() {
  int r;

  asm volatile( "svc %1     \n" // make system call SYS_FORK
                "mov %0, r0 \n" // assign r  = r0    ,% is for arguments
              : "=r" (r)
              : "I" (SYS_FORK)
              : "r0" );

  return r;
}

void exit( int x ) {
  asm volatile( "mov r0, %1 \n" // assign r0 =  x
                "svc %0     \n" // make system call SYS_EXIT
              :
              : "I" (SYS_EXIT), "r" (x)
              : "r0" );

  return;
}

void exec( const void* x ) {
  asm volatile( "mov r0, %1 \n" // assign r0 = x
                "svc %0     \n" // make system call SYS_EXEC
              :
              : "I" (SYS_EXEC), "r" (x)
              : "r0" );

  return;
}

int kill( int pid, int x ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 =  pid
                "mov r1, %3 \n" // assign r1 =    x
                "svc %1     \n" // make system call SYS_KILL
                "mov %0, r0 \n" // assign r0 =    r
              : "=r" (r)
              : "I" (SYS_KILL), "r" (pid), "r" (x)
              : "r0", "r1" );

  return r;
}

int cpipe( int start, int end ){
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = start
                "mov r1, %3 \n" // assign r1 = end
                "svc %1     \n" // make system call SYS_CPIPE
                "mov %0, r0 \n" // assign r0 = r
              : "=r" (r)
              :"I" (SYS_CPIPE), "r" (start), "r" (end)
              : "r0", "r1" );

  return r;
}

enum request_t readc( int id){
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = id
                "svc %1     \n" // make system call SYS_READC
                "mov %0, r0 \n" // assign r0 = r
              : "=r" (r)
              : "I" (SYS_READC), "r" (id)
              : "r0" );
  return r;
}

void writec( int id, enum request_t request){
  asm volatile( "mov r0, %1 \n" // assign r0 = id
                "mov r1, %2 \n" //assign r1 = request
                "svc %0     \n" // make system call SYS_WRITEC
              :
              : "I" (SYS_WRITEC), "r" (id), "r" (request)
              : "r0", "r1" );
  return ;
}

int getpid(){
  int r;

  asm volatile( "svc %1     \n" // make system call SYS_GETPID
                "mov %0, r0 \n" // assign r  = r0    ,% is for arguments
              : "=r" (r)
              : "I" (SYS_GETPID)
              : "r0" );
  return r;
}
