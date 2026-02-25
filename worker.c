//
// Created by Paul Corbin on 2/24/2026.
//
#include "clock.h"
#include "shared.h"
/*
 *main {
    *arg 1 = seconds
    *arg 2 = nanoseconds
    *print worker pid
    *print arguements
    *
    *Worker starting, PID:6577 PPID:6576
    *Called with:
    *Interval: 5 seconds, 500000 nanoseconds
    *
    *attach to shared memory
    *
    *calculate term time
    *
    *wait true {
       *WORKER PID:6577 PPID:6576
       *SysClockS: 6 SysclockNano: 45000000 TermTimeS: 11 TermTimeNano: 500100
       *--1 seconds have passed since starting
       *check if time to terminate
    *}
    *WORKER PID:6577 PPID:6576
    *SysClockS: 11 SysclockNano: 700000 TermTimeS: 11 TermTimeNano: 500100
    *--Terminating
 *}
 */

int main(int argc, char *argv[]) {
   //Should be 3 arguements otherwise error
   if (argc != 3) {
      fprintf(stderr, "Usage: ./worker seconds nanoseconds");
      exit(1);
   }

   int seconds = atoi(argv[1]);
   int nanoseconds = atoi(argv[2]);
   /*Worker starting, PID:6577 PPID:6576
   *Called with:
   *Interval: 5 seconds, 500000 nanoseconds*/
   printf("Worker starting, PID:%d PPID:%d\n", getpid(), getppid());
   printf("Called with:\n");
   printf("Interval: %d seconds, %d nanoseconds\n", seconds, nanoseconds);


   int shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | SHM_PERM);
   Clock *clock = (Clock *)shmat(shmid, NULL, 0);

   unsigned int termSeconds = clock->seconds + seconds;
   unsigned int termNanoseconds = clock->nanoseconds + nanoseconds;

   if (termNanoseconds > 1000000000) {
      termSeconds += 1;
      termNanoseconds -= 1000000000;
   }

   unsigned int startSeconds = clock->seconds;
   unsigned int startNanoseconds = clock->nanoseconds;

   while (1) {
      if ()
   }
}