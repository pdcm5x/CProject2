//
// Created by Paul Corbin on 2/24/2026.
//
#include "clock.h"
#include "pcb.h"
#include "shared.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAX_PROCS 20
#define BILLION 1000000000
/*
   *increment clock function (clock) {
      *increment by 10 ms
   *}
   *
   *main{
      *int shared memory id = shmget()
      *int max total processes
      *int simultaneously run
      *int launched
      *int currently running
      *print oss pid
      *initialize clock in shared memory
      *processtable[max processes]
         *while launched < total  {
         *increment clock
          *if launched < total and running < simultaneous amount {
          *pid_t pid fork process
          *if child process {
            *execute ./worker
          *}
          *update PCB table
          *total launched ++
          *total running++
         *}
         *int pid = waitpid(-1, &status, wnohang);
         *if (pid > 0) {
          *update process table
          *running--
         *}
      *}
      *print oss is done
      *delete shared memory
*}
*/
PCB processTable[MAX_PROCS];
void incrementClock(Clock *clock) {
   clock->nanoseconds += 10000000;
   if (clock->nanoseconds >= BILLION) {
      clock->nanoseconds -= BILLION;
      clock->seconds++;
   }
}
int main(int argc, char   *argv[]) {
   int opt;
   int n = 5;          // max simultaneous children (-n)
   int s = 2;          // initial children (-s)
   float t = 5.0;      // time limit in seconds (-t)
   float i_interval = 0.5; // interval between launches (-i)

   // parse command-line options
   while ((opt = getopt(argc, argv, "n:s:t:i:h")) != -1) {
      switch (opt) {
         case 'n': n = atoi(optarg); break;
         case 's': s = atoi(optarg); break;
         case 't': t = atof(optarg); break;
         case 'i': i_interval = atof(optarg); break;
         case 'h':
         default:
            printf("Usage: %s [-n max_simultaneous] [-s initial_children] [-t time_limit] [-i launch_interval]\n", argv[0]);
            exit(0);
      }
   }
   printf("OSS starting PID:%d PPID:%d\n", getpid(), getppid());
   //Shared memory
   int shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | SHM_PERM);
   if (shmid == -1) {
      perror("shmget failed");
      exit(1);
   }
   Clock *clock = (Clock *)shmat(shmid, NULL, 0);
   if (clock == (Clock *)-1) {
      perror("shmat failed");
      exit(1);
   }
   clock->nanoseconds = 0;
   clock->seconds = 0;
   memset(processTable, 0, MAX_PROCS * sizeof(PCB));
   int launched = 0;
   int running = 0;
   unsigned int lastLaunchSec = 0, lastLaunchNano = 0;
   while (launched < s || running > 0) {
      incrementClock(clock);
      //Check if we can launch another program
      unsigned long lastLaunch = lastLaunchSec * BILLION + lastLaunchNano;
      unsigned long currentTime = clock->seconds * BILLION + clock->nanoseconds;
      unsigned long elapsedSinceLastLaunch = currentTime - lastLaunch;
      if (launched < s && running < n && elapsedSinceLastLaunch >= (unsigned long)(i_interval * BILLION)) {
         pid_t pid = fork();
         if (pid == -1) {
            perror("fork failed");
            exit(1);
         }
         if (pid == 0) {
            int maxSec = (int)t;
            int maxNano = (int)((t - maxSec) * BILLION);
            //Translate to strings to use with execl
            char secStr[16], nanoStr[16];
            snprintf(secStr, sizeof(secStr), "%d", maxSec);
            snprintf(nanoStr, sizeof(nanoStr), "%d", maxNano);
            execl("./worker", "worker", secStr, nanoStr, NULL);
            perror("execl failed");
            exit(1);
         }
         //Update PCB
         for (int i = 0; i < MAX_PROCS; i++) {
            if (!processTable[i].occupied) {
               processTable[i].occupied = 1;
               processTable[i].pid = pid;
               processTable[i].startSeconds = clock->seconds;
               processTable[i].startNano = clock->nanoseconds;
               processTable[i].endingTimeSeconds = clock->seconds + (int)t;
               processTable[i].endingTimeNano = clock->nanoseconds + (int)((t - (int)t) * BILLION);
               if (processTable[i].endingTimeNano >= BILLION) {
                  processTable[i].endingTimeSeconds += 1;
                  processTable[i].endingTimeNano -= BILLION;
               }
               break;
            }
         }
         lastLaunchSec = clock->seconds;
         lastLaunchNano = clock->nanoseconds;
         running++;
         launched++;
      }
      int status;
      pid_t termPid;
      while ((termPid = waitpid(-1, &status, WNOHANG)) > 0) {
         for (int i = 0; i < MAX_PROCS; i++) {
            if (processTable[i].pid == termPid) {
               processTable[i].occupied = 0;
               break;
            }
         }
         running--;
      }
      if (clock -> nanoseconds % 500000000 == 0) {
         printf("OSS PID:%d SysClockS:%u SysClockNano:%u\n",getpid(),clock->seconds,clock->nanoseconds);
      }
   }
   printf("OSS terminating\n");
   shmdt(clock);
   shmctl(shmid, IPC_RMID, NULL);
   return 0;
}