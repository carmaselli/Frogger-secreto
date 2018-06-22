/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

static void* frogTimeThread (void* event);
static void printBoard(bool p2board[][DISSIZE]);



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

void* input_thread (void* event)//genera eventos de movimiento del joystick
{
  event_t *my_event = event; //asigno puntero para no tener que castear pcada vez que quiero usarlo
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
          my_event->flag = true;
          my_event->type = RIGHT_EVENT;
          trigger_lock_x = true;
          printf("RIGHT EVENT\n");
         }
         else if(my_coordinates.x < -JOY_THRESHOLD )
         {
          my_event->flag = true;
          my_event->type = LEFT_EVENT;
          trigger_lock_x = true;
          printf("LEFT EVENT\n");
         }
       }
       if(!trigger_lock_y)
       {
         if(my_coordinates.y > JOY_THRESHOLD )
         {
           my_event->flag = true;
           my_event->type = UP_EVENT;
           trigger_lock_y = true;
           printf("UP EVENT\n");
         }
         else if(my_coordinates.y < -JOY_THRESHOLD )
         {
           my_event->flag = true;
           my_event->type = DOWN_EVENT;
           trigger_lock_y = true;
           printf("DOWN EVENT\n");
         }
       }
       if(my_switch == J_PRESS && !switch_lock)
    	{
        my_event->flag = true;
        my_event->type = ENTER_EVENT;
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
    bool board[DISSIZE][DISSIZE];
    int i,j;
    for( i = 0 ; i < DISSIZE ; i++ )
    {
      for( j = 0 ; j < DISSIZE ; j++ )
      {
        board[i][j] = false; //inicializo tablero apagado
      }
    }
    gameData_t *pGameData = pointer;
    display_init(); // inicializacion del display
    set_display_axis(NORMAL);
    display_clear();
    bool frogTimer = false;

    pthread_t frogTid;
    pthread_create(&frogTid,NULL,frogTimeThread,&frogTimer);    //creacion de timer para parpadeo de la rana
    bool toggle = false;    //variable para el parpadeo de la rana

    infinite_loop
    {
      while(start menu)
      {
        //ver teclado
        //imprimir menus correspondientes
        //realizar accion
        //avisar a main
        //ahora a escribir gato =)
      }



      while( pGameData->stateID == GAME )//mover autos,VER CARS_ROUTINE
      {

        if(carsTimer)
        {
          carsTimer = false;

        }


        printBoard(board);  //VER ACA COMO ES LA ESTRUCTURA GAME DATA Y OJO CON EL NIVEL DEL PUNTERO//Escribe en el display el estado actual de autos y troncos
        display_update();

        if( checkCollision(pGameData->frog,board) )
        {
            AVISAR QUE HUBO EVENTO HAY QUE CAMBIAR GAME DATA    //verifica si la rana choco
        }
        else if( checkWin(pGameData->frog,board) );atom://teletype/portal/aed65d0e-56e8-4c3f-8f15-2c19721d350d

        {
            AVISAR QUE SUBIO DE NIVEL!!!!!!!!!!!!!!

        }



        if(frogTimer)
        {
            toggle = !toggle;
            display_write(pGameData->frog.x,pGameData->frog.y,toggle);  //prende/apaga la posicion de la rana
          //  display_update();   //SE PUEDE SACAR SIEMPRE QUE ESTO ESTE CERCA DEL FINAL DEL LOOP
        	  frogTimer = false;
        }

        display_update();
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
Recibe: puntero a la posicion de la rana y puntero al arreglo con la posicion de los autos
Devuelve 1 si la rana choco, 0 si no choco. */
bool checkCollision(frog_t frogCoords,bool board[][DISSIZE])
{
  return board[frogCoords.x][frogCoords.y]; //OJO!!!
}

/*checkWin:
Recibe: puntero a la posicion de la rana y puntero al arreglo con los autos, troncos, y posiciones donde puede llegar al final
Devuelve: 1 si paso de nivel(completo todos los lugares para llegar), 0 si no.
Funcionamiento: Se fija si la rana esta en la ultima fila, si esta gano porque se acaba de fijar que no haya chocado
                Despues se fija si paso de nivel, es decir, si ya cayo en todos los otros lugares  */
bool checkWin(frog_t frogCoords, bool board[][DISSIZE])
{
  int i;
  if(!frogCoords.y)   //si la rana esta en la ultima fila(fila 0)
  {
    bool check = false;
    board[frogCoords.x][frogCoords.y] = 1;    //deja prendido el lugar adonde llego la rana
    frogCoords.y = INIT_Y;    //OJO!!
    frogCoords.x = INIT_X;     //devuelve la rana a su posicion inicial
    for( i=0 ; (i<DISSIZE) && !check ; i++ )
    {
      check = !board[0][i];   //OJOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
    }
    return check;
  }
  else
  {
    return 0;
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



/*****************************/ /*TIMERRRRRRRRR*/ /*******************************/
    gameData_t *my_data = pArg; //asigno puntero para no tener que castear pcada vez que quiero usarlo
    infinite_loop
    {
        if(my_data->event.timerFlag == false)//OJO esto es si queres que espere a que el main lo vea, si no te importa sacalo, pero cuidado hay que poner semaforos y/o MUTEX (creo que mutex)
        {
            usleep(TIME_MS*1000);
            my_data->event.timerFlag = true;
        }
    }
}







/****************************MOVIMIENTO DE AUTOS*********************************/
void cars_routine(void *pArg)
{
    gameData_t *pData = pArg;
    static int dividersMax[BOARD_SIZE] = {0, 15, 20, 8, 15, 20, 8, 15, 0, 12, 7, 12, 10, 7, 10, 0}; // Cuando se suba de nivel, alguno de estos máximos se decrementará para hacer que el ciclo de avance de el carril correspondiente sea más rápido.
    static int dividers[BOARD_SIZE] = {0, 15, 20, 8, 15, 20, 8, 15, 0, 12, 7, 12, 10, 7, 10, 0}; // Ante un evento de timer, se decrementa el divider de cada carril, logrando así que cada carril tenga su ciclo de timer, cuando el divider llega a 0.
    boolean_t ways[BOARD_SIZE] = {0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0};
    int row = 0;
    srand(time(NULL));

    if(pData->levelUp) // Si se tiene que subir de nivel, se efectua un cambio en el máximo de los divisores.
    {
        pData->levelUp = !(pData->levelUp); // Se evita que se suba de nivel nuevamente.
        while((dividersMax[row] <= 1))
        {
            row = rand()%16; // Se selecciona al azar uno de los carriles a aumetar su velocidad. No se aumenta la volocidad de los que ya van a la velocidad del clock.
        }
        dividersMax[row]--;

        pData->frog.x = 8;
        pData->frog.y = 15; // Se reinicia la posición de la rana.
    }
    else // En cambio, si la rutina fue llamada por evento de timer, se realiza el decremento de los dividers.
    {
        for(row = 0; row < BOARD_SIZE; row++)
        {
            if(dividersMax[row]) // Si no se está en una fila con divider maximo 0...
            {
                dividers[row]--;
                if(!dividers[row]) // Si se cumplió el ciclo, se mueven los autos.
                {
                    shift_handler((*pData->pBoard), ways[row], row);
                    dividers[row] = dividersMax[row]; // Se resetea el ciclo con el maximo de cada divider.
                }
            }
        }
    }
}


void shift_handler(boolean_t row[BOARD_SIZE][BOARD_SIZE], boolean_t way, int row_num)
{
    if(way) // Si way es 1, se gira a la derecha.
    {
        shift_right_row(row, row_num);
    }
    else // Se gira a izquierda.
    {
        shift_left_row(row, row_num);
    }
}

void shift_right_row(boolean_t row[BOARD_SIZE][BOARD_SIZE], int row_num)
{
    boolean_t aux1 = row[BOARD_SIZE-1][row_num], aux2;
    int i;
    for(i = 0; i < BOARD_SIZE; i++)
    {
        aux2 = row[i][row_num];
        row[i][row_num] = aux1;
        aux1 = aux2;
    }
}

void shift_left_row(boolean_t row[BOARD_SIZE][BOARD_SIZE], int row_num)
{
    boolean_t aux1 = row[0][row_num], aux2;
    int i;
    for(i = 0; i < BOARD_SIZE; i++)
    {
        aux2 = row[15-i][row_num];
        row[15-i][row_num] = aux1;
        aux1 = aux2;
    }
}