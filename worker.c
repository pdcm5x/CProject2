//
// Created by Paul Corbin on 2/24/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include "clock.h"
#include "shared.h"
int main(int argc, char *argv[]) {
    // Validate arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: ./WORKER seconds nanoseconds\n");
        exit(1);
    }

    int secInterval = atoi(argv[1]);
    int nanoInterval = atoi(argv[2]);

    // Print starting info
    printf("WORKER starting, PID:%d PPID:%d\n", getpid(), getppid());
    printf("Called with:\n");
    printf("Interval: %d seconds, %d nanoseconds\n", secInterval, nanoInterval);

    // Attach to shared memory
    int shmid = shmget(SHM_KEY, SHM_SIZE, SHM_PERM);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    Clock *clock = (Clock *)shmat(shmid, NULL, 0);
    if (clock == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // Calculate termination time
    unsigned int termSeconds = clock->seconds + secInterval;
    unsigned int termNanoseconds = clock->nanoseconds + nanoInterval;
    if (termNanoseconds >= 1000000000) {
        termSeconds += 1;
        termNanoseconds -= 1000000000;
    }

    unsigned int lastSecondCheck = clock->seconds;
    unsigned int secondsPassed = 0;

    // Wait loop until termination time
    while (1) {
        // Check if termination time reached
        if (clock->seconds > termSeconds ||
           (clock->seconds == termSeconds && clock->nanoseconds >= termNanoseconds)) {
            printf("WORKER PID:%d PPID:%d\n", getpid(), getppid());
            printf("SysClockS: %u SysClockNano: %u TermTimeS: %u TermTimeNano: %u\n",
                   clock->seconds, clock->nanoseconds, termSeconds, termNanoseconds);
            printf("--Terminating\n");
            break;
        }

        // Print every second passed
        if (clock->seconds - lastSecondCheck >= 1) {
            secondsPassed += clock->seconds - lastSecondCheck;
            printf("WORKER PID:%d PPID:%d\n", getpid(), getppid());
            printf("SysClockS: %u SysClockNano: %u TermTimeS: %u TermTimeNano: %u\n",
                   clock->seconds, clock->nanoseconds, termSeconds, termNanoseconds);
            printf("--%u seconds have passed since starting\n", secondsPassed);
            lastSecondCheck = clock->seconds;
        }
    }

    // Detach shared memory
    shmdt(clock);
    return 0;
}