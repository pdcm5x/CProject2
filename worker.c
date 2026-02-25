//
// Created by Paul Corbin on 2/24/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
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


   int shmid = shmget(SHM_KEY, SHM_SIZE, SHM_PERM);
   Clock *clock = (Clock *)shmat(shmid, NULL, 0);

   unsigned int termSeconds = clock->seconds + seconds;
   unsigned int termNanoseconds = clock->nanoseconds + nanoseconds;

   if (termNanoseconds >= 1000000000) {
      termSeconds += 1;
      termNanoseconds -= 1000000000;
   }

   unsigned int lastSeconds = clock->seconds;
   unsigned int lastNanoseconds = clock->nanoseconds;
   unsigned int passedSeconds = 0;
   while (1) {
      //Check to stop loop
      if (clock->seconds > termSeconds || (clock->nanoseconds >= termNanoseconds && clock->seconds == termSeconds)) {
         printf("WORKER PID:%d PPID:%d\n", getpid(), getppid());
         printf("SysClockS: %u SysclockNano: %u TermTimeS: %u TermTimeNano: %u\n", clock->seconds, clock->nanoseconds, termSeconds, termNanoSeconds);
         printf("--Terminating\n");
         break;
      }
      //Check if a second has passed.
      if (clock->seconds - lastSeconds >= 1) {
         passedSeconds += clock->seconds - lastSeconds;
         printf("WORKER PID:%d PPID:%d\n", getpid(), getppid());
         printf("SysClockS: %u SysclockNano: %u TermTimeS: %u TermTimeNano: %u\n", clock->seconds, clock->nanoseconds, termSeconds, termNanoSeconds);
         printf("--%u seconds have passed since starting\n", passedSeconds);
         lastSeconds = clock->seconds;
      }
   }
   shmdt(clock);
   return 0;
}