*
 */*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "modulorpi.h"
#include "eventQueue.h"
#include "gameStructs.h"

static void printBoard(bool p2board[][DISSIZE]);
static bool checkWin(frog_t *frogCoords, bool board[][DISSIZE]);
static bool checkCollision(frog_t *frogCoords,bool board[][DISSIZE]);
static void moveFrog(uint16_t where,frog_t *frogCoords);
static void cars_routine(bool board[][DISSIZE],frog_t *frogCoords);
static void shift_handler(bool board[DISSIZE][DISSIZE], bool way, int row_num);
static void shift_right_row(bool row[DISSIZE][DISSIZE], int row_num);
static void shift_left_row(bool row[DISSIZE][DISSIZE], int row_num);

/*int main(void)
{
  event_t evento;
  evento.flag = false;
  gameData_t gameData;
  gameData.frog.x = 0;
  gameData.frog.y = 0;
  pthread_t tinp,tout;
  bool board[DISSIZE][DISSIZE];
  gameData.pBoard = &board;  //POSIBLE QUILOMBO
  int i,j;
  for( i = 0 ; i < DISSIZE ; i++ )
  {
    for( j = 0 ; j < DISSIZE ; j++ )
    {
      gameData.pBoard[0][i][j] = false; //inicializo tablero apagado
    }
  }
  pthread_create(&tinp,NULL,input_thread,&evento);
  pthread_create(&tout,NULL,output_thread,&gameData);
  changemode(BUFFERED_OFF);
  while(!kbhit())
  {
    if(evento.flag)
    {
      evento.flag = false;
      if(evento.type == RIGHT_EVENT)
      {
        gameData.frog.x++;
      }
      else if(evento.type == LEFT_EVENT)
      {
        gameData.frog.x--;
      }
      else if(evento.type == UP_EVENT)
      {
        gameData.frog.y++;
      }
      else if(evento.type == DOWN_EVENT)
      {
        gameData.frog.y--;
      }
      else if(evento.type == ENTER_EVENT)
      {
      }
      if(gameData.frog.x > MAX_X)
      {
        gameData.frog.x = MAX_X;
      }
      else if(gameData.frog.x < MIN_X)
      {
        gameData.frog.x = MIN_X;
      }
      if(gameData.frog.y > MAX_Y)
      {
        gameData.frog.y = MAX_Y;
      }
      if(gameData.frog.y < MIN_Y)
      {
        gameData.frog.y = MIN_Y;
      }
    }
  }
  changemode(BUFFERED_ON);
  display_clear();
  return 0;
}*/

void* input_thread (void* eventQueue)//genera eventos de movimiento del joystick
{
  event_queue_t *my_event_queue = eventQueue; //asigno puntero para no tener que castear pcada vez que quiero usarlo
  jcoord_t my_coordinates;
  jswitch_t my_switch = J_NOPRESS;
  bool trigger_lock_x = false;
  bool trigger_lock_y = false;
  bool switch_lock = false;

  joy_init();
  set_joy_axis(JOY_ROTATE);	// OJO considero que usamos joystick abajo y no a la izq
  set_joy_direction(J_INV_TRUE,J_INV_TRUE);

  infinite_loop
  {
    joystick_update();
    my_switch=joystick_get_switch_value();//recibe valores actuales del joystick y el boton
    my_coordinates = joystick_get_coord();

    if(my_event->flag != true ) //generador de eventos
    {
       if(!trigger_lock_x)
       {
         if(my_coordinates.x > JOY_THRESHOLD )
         {
          emit_event(my_event_queue , RIGHT_EVENT);
          trigger_lock_x = true;
          printf("RIGHT EVENT\n");
         }
         else if(my_coordinates.x < -JOY_THRESHOLD )
         {
          emit_event(my_event_queue , LEFT_EVENT);
          trigger_lock_x = true;
          printf("LEFT EVENT\n");
         }
       }
       if(!trigger_lock_y)
       {
         if(my_coordinates.y > JOY_THRESHOLD )
         {
           emit_event(my_event_queue , UP_EVENT);
           trigger_lock_y = true;
           printf("UP EVENT\n");
         }
         else if(my_coordinates.y < -JOY_THRESHOLD )
         {
           emit_event(my_event_queue , DOWN_EVENT);
           trigger_lock_y = true;
           printf("DOWN EVENT\n");
         }
       }
       if(my_switch == J_PRESS && !switch_lock)
    	{
        emit_event(my_event_queue , ENTER_EVENT);
        switch_lock = true;
        printf("ENTER EVENT\n");
    	}
    }
    if (trigger_lock_x && my_coordinates.x < JOY_THRESHOLD && my_coordinates.x > -JOY_THRESHOLD)  //bloqueo de lectura para evitar que se envie el evento si se mantiene presionado
    {
      trigger_lock_x = false;
      //printf("TRIGGERLOCKX OFF\n");
    }
    if (trigger_lock_y && my_coordinates.y < JOY_THRESHOLD && my_coordinates.y > -JOY_THRESHOLD)  //bloqueo de lectura para evitar que se envie el evento si se mantiene presionado
    {
      trigger_lock_y = false;
      //printf("TRIGGERLOCKY OFF\n");
    }
    if (switch_lock && my_switch == J_NOPRESS)  //bloqueo de lectura para evitar que se envie el evento si se mantiene presionado
    {
      switch_lock = false;
      //printf("SWITCHLOCK OFF\n");
    }
  }
}

/*output_thread (de Raspberry Pi)
Recibe un puntero a la estructura gameData que contiene la informacion del juego
Se encarga de:
-manejar los menús juego
-Mover los autos y los troncos
-Imprimir todo en el display de 16x16
-Informar al kernel cuando hubo un choque
-Hacer parpadear la rana para diferenciarla de los autos (para eso usa el thread frogTimeThread)
Nota: no se encarga de mover la rana en el funcionamiento interno del juego, solo imprime su movimiento */

void* output_thread(void* pointer)
{
    bool carsBoard[DISSIZE][DISSIZE],disBoard[DISSIZE][DISSIZE];
    frog_t frogCoords;
    int i,j;
    for( i = 0 ; i < DISSIZE ; i++ )
    {
      for( j = 0 ; j < DISSIZE ; j++ )
      {
        board[i][j] = false; //inicializo tablero apagado
      }
    }
    gameData_t *pGameData = pointer;
    int maxPosition = INIT_Y;
    
    display_init(); // inicializacion del display
    set_display_axis(NORMAL);
    display_clear();
    
    bool frogTimer = false, dispTimer = false;
    pthread_t frogTid, dispTid;
    pthread_create(&frogTid,NULL,frogTimeThread,&frogTimer);    //creacion de timer para parpadeo de la rana
    pthread_create(&dispTid,NULL,dispTimeThread,&dispTimer);
    bool toggle = false;    //variable para el parpadeo de la rana

    while(!pGameData->quitGame)
    {
      while(pGameData->currentState->stateID >= START_PLAY_ID && pGameData->currentState->stateID <= START_QUIT_ID)
      {
        
         //ver teclado
        //imprimir menus correspondientes
        //realizar accion
        //avisar a main
        //ahora a escribir gato =)
      }
      


      while( pGameData->stateID == GAME_ID )//mover autos,VER CARS_ROUTINE
      {
          
        if(carsTimer)
        {
          cars_routine(carsBoard,&frogCoords);  //mueve los autos y si hace falta la rana
          carsTimer = false;
        }
        if(pGameData->moveFrog.flag)
        {
            moveFrog(pGameData->moveFrog.where,&frogCoords); //HABRIA QUE CAMBIAR GAMEDATA
            if(maxPosition > frogCoords.y)
            {
                maxPosition = frogCoords.y;     //se fija si avanzo mas que antes, en caso afirmativo le avisa al main para actualizar el puntaje
                if( !emit_event(pGameData->pEventQueue,FORWARD_EVENT) )
                {
                    printf("Coludn't emit event\n");
                }    
            }    
        }    
        
        printBoard(carsBoard);  //Escribe en el display el estado actual de autos y troncos
        
        if( checkCollision(&frogCoords,carsBoard) )         //FIJARSE EL ORDEN! SI PERDIO CAPAZ HAYA QUE PONER UN BREAK
        {
            maxPosition = INIT_Y;
            if( !emit_event(pGameData->pEventQueue,COLLISION_EVENT) )   //si la rana choco, le avisa al main
            {
                printf("Coludn't emit event\n");
            }  
        }
        else if( checkWin(&frogCoords,carsBoard) )
        {
            maxPosition = INIT_Y;
            if( !emit_event(pGameData->pEventQueue,FORWARD_EVENT) )
            {
                printf("Coludn't emit event\n");
            }         
            //SEM WAIT LEVEL UP O PREGUNTAR EN OTRO LADO LEVEL UP, POR EJEMPLO ANTES DE MOVER LOS AUTOS
            if(pGameData->levelUp)
            {
                cars_routine(NULL,&frogCoords);
                pGameData->levelUp = 0;
            }
            
        }
              
        if(dispTimer)
        {
            if(!frogCounter--)
            {
                frogCounter = FROG_REFRESH;
                toggle = !toggle;
                display_write(pGameData->frog.x,pGameData->frog.y,toggle);  //prende/apaga la posicion de la rana                
            }    
            display_update();
        }    
      }
      while(pausa)
      {
          //revisar teclado
          //SOLO 2 OPCIONES INICIO Y RETURN
          //RETURN SALE DEL WHILE SIN HACER NADA (CAMBIAR ESTADO A GAME)
          //INICIO VUELVE AL MENU PRINCIPAL DEL JUEGO( CAMBIAR ESTADO A MENU Y SALIR DEL WHILE)
      }

    }
}

/*checkCollision:
Recibe: posicion de la rana y puntero al arreglo con la posicion de los autos
Devuelve 1 si la rana choco, 0 si no choco.
    Ademas, si choco la devuelve a su posicion inicial */
bool checkCollision(frog_t *frogCoords,bool board[][DISSIZE])
{
  if(board[frogCoords->x][frogCoords->y]) //OJO X E Y!!!
  {
      frogCoords->x = INIT_X;
      frogCoords->y = INIT_Y;
      return true;
  }    
  else
  {
      return false;
  }    
}

/*checkWin:
Recibe: puntero a la posicion de la rana y puntero al arreglo con los autos, troncos, y posiciones donde puede llegar al final
Devuelve: 1 si llego al final, 0 si no.
Funcionamiento: Se fija si la rana esta en la ultima fila, si esta gano porque se acaba de fijar que no haya chocado */

bool checkWin(frog_t *frogCoords, bool board[][DISSIZE])
{
  int i;
  if(!frogCoords->y)   //si la rana esta en la ultima fila(fila 0)
  {
    board[frogCoords->x][frogCoords->y] = 1;    //deja prendido el lugar adonde llego la rana
    frogCoords->y = INIT_Y;    //OJO!!
    frogCoords->x = INIT_X;     //devuelve la rana a su posicion inicial
    return true;
  }
  else
  {
    return false;
  }

}

/*printBoard:
Recibe un puntero al primer elemento de un arreglo de bools
Escribe en un display de 16x16 el contenido de ese arreglo
Nota: debe estar inicializado el display con las herramientas de <disdrv.h> */
void printBoard(bool p2board[][DISSIZE])
{
  int i,j;
  for( i=0 ; i < DISSIZE ; i++ )
  {
    for( j=0 ; j < DISSIZE ; j++)
    {
      display_write(i,j,p2board[i][j]);
    }
  }
}

void moveFrog(uint16_t where,frog_t *frogCoords)
{
  switch(where)
      case FROG_UP:
              frogCoords->y--;
          break;
      case FROG_DOWN:        
              frogCoords->y--;
          break;    
      case FROG_RIGHT:
              frogCoords->x++;  
          break;    
      case FROG_LEFT:
              frogCoords->x--;
          break;    
}   








/****************************MOVIMIENTO DE AUTOS*********************************/

/*cars_routine
 * Recibe un puntero a un arreglo con la posicion de los autos y un puntero a la posicion de la rana en X
 * Si se subio de nivel (enviar NULL como primer parametro), aumenta la velocidad del movimiento de los autos
 * Sino, 
 * Recibe NULL si se subio de nivel*/

void cars_routine(bool carsBoard[][DISSIZE],frog_t *frogCoords)
{
    static int dividersMax[DISSIZE] = {0, 15, 20, 8, 15, 20, 8, 15, 0, 12, 7, 12, 10, 7, 10, 0}; // Cuando se suba de nivel, alguno de estos máximos se decrementará para hacer que el ciclo de avance de el carril correspondiente sea más rápido.
    static int dividers[DISSIZE] = {0, 15, 20, 8, 15, 20, 8, 15, 0, 12, 7, 12, 10, 7, 10, 0}; // Ante un evento de timer, se decrementa el divider de cada carril, logrando así que cada carril tenga su ciclo de timer, cuando el divider llega a 0.
    bool ways[DISSIZE] = {0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0};
    int row = 0;

    if(!carsBoard) // Si se tiene que subir de nivel, se efectua un cambio en el máximo de los divisores.
    {
        for(row = 0 ; row < DISSIZE ; row++)
        {
            if(dividersMax[row] > 1)
            {
                dividersMax[row]--;
            }    
        }    
        frogCoords->x = INIT_X;
        frogCoords->y = INIT_Y; // Se reinicia la posición de la rana.
    }
    else // En cambio, si la rutina fue llamada por evento de timer, se realiza el decremento de los dividers.
    {
        for(row = 0; row < DISSIZE; row++)
        {
            if(dividersMax[row]) // Si no se está en una fila con divider maximo 0...
            {
                dividers[row]--;
                if(!dividers[row]) // Si se cumplió el ciclo, se mueven los autos.
                {    
                    shift_handler(carsBoard, ways[row], row);
                    dividers[row] = dividersMax[row]; // Se resetea el ciclo con el maximo de cada divider.
                    if(frogCoords->y == row && row > 0 && row < 8 )
                    {
                        if(ways[row])
                        {
                            frogCoords->x++;     //si ademas esta moviendo troncos, mueve la rana junto con los troncos
                        }
                        else
                        {
                            frogCoords->x--;
                        }    
                    }
                }
            }
        }
    }
}

void shift_handler(bool board[DISSIZE][DISSIZE], bool way, int row_num)
{
    if(way) // Si way es 1, se gira a la derecha.
    {            
        shift_right_row(board, row_num);
    }
    else // Se gira a la izquierda.
    {
        shift_left_row(board, row_num);
    }
}

void shift_right_row(bool row[DISSIZE][DISSIZE], int row_num)
{
    bool aux1 = row[DISSIZE-1][row_num], aux2;
    int i;
    for(i = 0; i < DISSIZE; i++)
    {
        aux2 = row[i][row_num];
        row[i][row_num] = aux1;
        aux1 = aux2;
    }
}

void shift_left_row(bool row[DISSIZE][DISSIZE], int row_num)
{
    bool aux1 = row[0][row_num], aux2;
    int i;
    for(i = 0; i < DISSIZE; i++)
    {
        aux2 = row[15-i][row_num];
        row[15-i][row_num] = aux1;
        aux1 = aux2;
    }
}
