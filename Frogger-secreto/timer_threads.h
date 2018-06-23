/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   timer_threads.h
 * Author: nico
 *
 * Created on June 23, 2018, 10:21 AM
 */

#ifndef TIMER_THREADS_H
#define TIMER_THREADS_H

#include <unistd.h>
#include <stdbool.h>

#define FROGTIME 1000
#define infinite_loop while(1)
#define ONE_SEC 1000000
#define FPS 45

void* carsTimeThread (void* event);
void* dispTimeThread(void* pArg);

#endif /* TIMER_THREADS_H */

