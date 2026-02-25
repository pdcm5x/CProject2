//
// Created by Paul Corbin on 2/24/2026.
//
#include "clock.h"
#include "pcb.h"
#define MAX_PROCS = 20
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

   return 0;
}