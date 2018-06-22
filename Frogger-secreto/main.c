/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: nico
 *
 * Created on June 22, 2018, 5:43 PM
 */

#include <stdio.h>
#include <stdlib.h>

/*Forward declarations*/
extern state_t startMenuViewScoreBoard[3+1];
extern state_t startMenuQuit[2+1];
extern state_t scoreBoard[3+1];
extern state_t game[8+1];
extern state_t pauseMenuResume[3+1];
extern state_t pauseMenuBackToStart[3+1];
extern state_t saveScore1Char[3+1];
extern state_t saveScore2Char[3+1];
extern state_t saveScore3Char[3+1];


int main(void) 
{
    
    state_t startMenuPlayGame[3+1] = 
    {
        {DOWN_EVENT,startMenuViewScoreboard,non_act_routine,START_PLAY_ID},
        {UP_EVENT,startMenuQuit,non_act_routine,START_PLAY_ID},
        {ENTER_EVENT,game,start_game,START_PLAY_ID},
        {END_TABLE,startMenuPlayGame,non_act_routine,START_PLAY_ID}
    };        
    state_t startMenuViewScoreBoard[3+1] =
    {
        {DOWN_EVENT,startMenuQuit,non_act_routine,START_SCOREBOARD_ID},
        {UP_EVENT,startMenuPlayGame,non_act_routine,START_SCOREBOARD_ID},
        {ENTER_EVENT,scoreBoard,non_act_routine,START_SCOREBOARD_ID},
        {END_TABLE,startMenuViewScoreBoard,non_act_routine,START_SCOREBOARD_ID}
    };
    state_t startMenuQuit[2+1] = 
    {
        {DOWN_EVENT,startMenuPlayGame,non_act_routine,START_QUIT_ID},
        {UP_EVENT,startMenuViewScoreBoard,non_act_rotuine,START_QUIT_ID},
        {ENTER_EVENT,NULL,endGame,START_QUIT_ID},
        {END_TABLE,startMenuQuit,non_act_routine,START_QUIT_ID}
    };
    state_t scoreBoard[3+1] = 
    {
        {DOWN_EVENT,scoreBoard,showNextScore,SCORE_BOARD_ID},
        {UP_EVENT,scoreBoard,showPreviousScore,SCORE_BOARD_ID},
        {ENTER_EVENT,startMenuViewScoreBoard,non_act_routine,SCORE_BOARD_ID},
        {END_TABLE,scoreBoard,non_act_routine,SCORE_BOARD_ID}
    };
    state_t game[8+1] =
    {
        {DOWN_EVENT,game,frog_down,GAME_ID},
        {UP_EVENT,game,frog_up,GAME_ID},
        {LEFT_EVENT,game,frog_left,GAME_ID},
        {RIGHT_EVENT,game,frog_right,GAME_ID},
        {ENTER_EVENT,pauseMenuResume,non_act_routine,GAME_ID},
        {ARRIVE_EVENT,game,checkLevelUp,GAME_ID},
        {COLLISION_EVENT,game,checkLives,GAME_ID},
        {FORWARD_EVENT,game,update_score,GAME_ID},
        {END_TABLE,game,non_act_routine,GAME_ID}
    };
    state_t pauseMenuResume[3+1] =
    {
        {DOWN_EVENT,pauseMenuBackToStart,non_act_routine,PAUSE_RESUME_ID},
        {UP_EVENT,pauseMenuBackToStart,non_act_routine,PAUSE_RESUME_ID},
        {ENTER_EVENT,game,non_act_routine,PAUSE_RESUME_ID},
        {END_TABLE,pauseMenuResume,non_act_routine,PAUSE_RESUME_ID}
    };
    state_t pauseMenuBackToStart[3+1] =
    {
        {UP_EVENT,pauseMenuResume,non_act_routine,PAUSE_RESTART_ID},
        {DOWN_EVENT,pauseMenuResume,non_act_routine,PAUSE_RESTART_ID},
        {ENTER_EVENT,startMenuPlayGame,non_act_routine,PAUSE_RESTART_ID},
        {END_TABLE,pauseMenuBackToStart,non_act_routine,PAUSE_RESTART_ID}
    };
    state_t saveScoreChar[3+1] =
    {
        {DOWN_EVENT,saveScoreChar,fst_letter_down,SAVE_SCORE_ID},
        {UP_EVENT,saveScoreChar,fst_letter_up,SAVE_SCORE_ID},
        {LEFT_EVENT,saveScoreChar,previousChar,SAVE_SCORE_ID},
        {RIGHT_EVENT,saveScoreChar,nextChar,SAVE_SCORE_ID},
        {ENTER_EVENT,startMenuPlayGame,saveScore,SAVE_SCORE_ID},
        {END_TABLE,saveScoreChar,non_act_routine,SAVE_SCORE_ID}
    };
    
    return (EXIT_SUCCESS);
}

