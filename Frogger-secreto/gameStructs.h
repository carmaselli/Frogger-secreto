/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   gameStructs.h
 * Author: nico
 *
 * Created on June 22, 2018, 5:54 PM
 */

#ifndef GAMESTRUCTS_H
#define GAMESTRUCTS_H

#include <stdbool.h>

typedef struct
{
    bool flag;
    int type;
    bool timerFlag;
}event_t;

typedef struct
{
    char player[3];
    uint score;
} scorer_t;
typedef struct STATE
{
    event_t event;
    struct STATE *nextState;
    void (*actionRoutine)(void *);
    int stateID;
} state_t;

typedef struct
{
    uint8_t x;
    uint8_t y;
}frog_t;

typedef struct
{
    int lives;
    bool levelUp;
    bool frogHit;
    bool quitGame;
    bool (*pBoard)[16][16];
    frog_t frog;
    char player[3];
    uint score;
    scorer_t (*pTop10)[10];
    state_t *currentState;
    event_t event;
}gameData_t;


#endif /* GAMESTRUCTS_H */

