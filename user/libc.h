#ifndef __LIBC_H
#define __LIBC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Define a type that that captures a Process IDentifier (PID).

typedef int pid_t;

enum request_t {
  okay,
  notokay,
  eat,
  finish,
};

/* The definitions below capture symbolic constants within these classes:
 *
 * 1. system call identifiers (i.e., the constant used by a system call
 *    to specify which action the kernel should take),
 * 2. signal identifiers (as used by the kill system call),
 * 3. status codes for exit,
 * 4. standard file descriptors (e.g., for read and write system calls),
 * 5. platform-specific constants, which may need calibration (wrt. the
 *    underlying hardware QEMU is executed on).
 *
 * They don't *precisely* match the standard C library, but are intended
 * to act as a limited model of similar concepts.
 */

#define SYS_YIELD     ( 0x00 )
#define SYS_WRITE     ( 0x01 )
#define SYS_READ      ( 0x02 )
#define SYS_FORK      ( 0x03 )
#define SYS_EXIT      ( 0x04 )
#define SYS_EXEC      ( 0x05 )
#define SYS_KILL      ( 0x06 )
#define SYS_CPIPE     ( 0x07 )
#define SYS_READC     ( 0x08 )
#define SYS_WRITEC    ( 0x09 )
#define SYS_GETPID    ( 0xA  )
#define SYS_ISFULL    ( 0xB  )
#define SYS_CHANGEC   ( 0xC  )
#define SYS_SHOWPHIL  ( 0xD  )


#define SIG_TERM      ( 0x00 )
#define SIG_QUIT      ( 0x01 )

#define EXIT_SUCCESS  ( 0 )
#define EXIT_FAILURE  ( 1 )

#define  STDIN_FILENO ( 0 )
#define STDOUT_FILENO ( 1 )
#define STDERR_FILENO ( 2 )

// convert ASCII string x into integer r
extern int  atoi( char* x        );
// convert integer x into ASCII string r
extern void itoa( char* r, int x );

// cooperatively yield control of processor, i.e., invoke the scheduler
extern void yield();

// write n bytes from x to   the file descriptor fd; return bytes written
extern int write( int fd, const void* x, size_t n );
// read  n bytes into x from the file descriptor fd; return bytes read
extern int  read( int fd,       void* x, size_t n );

// perform fork, returning 0 iff. child or > 0 iff. parent process
extern int  fork();
// perform exit, i.e., terminate process with status x
extern void exit(       int   x );
// perform exec, i.e., start executing program at address x
extern void exec( const void* x );

// signal process identified by pid with signal x
extern int  kill( pid_t pid, int x );

// creates a new pipe with one process at the start and another one at the end
extern int cpipe( int start, int end);

// reads from the channel
extern enum request_t readc(int id);

//Writes in a channel
extern void writec(int id,enum request_t request);

// returns the current philosopher process id
extern int getpid();

// Checks whether something was written in a channel
extern int isfull(int id);

//Make channel full or empty
extern void changec(int id, int x);

// Print when a philosopher is done in the frame buffer
extern void showphil(int x, int y, int id);

#endif
