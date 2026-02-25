//
// Created by Paul Corbin on 2/24/2026.
//
#ifndef PCB_H
#define PCB_H
#include <sys/types.h>
#include <unistd.h>
typedef struct {
    int occupied;
    pid_t pid;
    int startSeconds;
    int startNano;
    int endingTimeSeconds;
    int endingTimeNano;
} PCB;
#endif