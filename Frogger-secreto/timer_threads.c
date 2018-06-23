/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "timer_threads.h"

/*frogTimeThread
Timer para el parpadeo de la rana */
void* frogTimeThread (void* p2timer)
{
  infinite_loop
  {
   if(*(bool*) p2timer == false)  //solo arranca nuevamente cuando hayan leido el evento
   {
     usleep(FROGTIME);
     *(bool*)p2timer = true;
   }
  }
  return NULL;
}


/*dispTimeThread
 Timer para la actualizacion del display*/ 
void* dispTimeThread(void* pArg)
{
    bool *pTimerFlag = pArg; //asigno puntero para no tener que castear pcada vez que quiero usarlo
    infinite_loop
    {
        usleep(ONE_SEC/FPS);
        *timerFlag = true;        
    }
}       