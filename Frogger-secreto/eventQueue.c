#include "eventQueue.h"


#define QUEUE_SIZE 3 //cambiar este define por la cantidad de posiciones deseadas en la cola






/*int main (void)
{
  event_queue_t my_event_queue = create_queue();
  if( (my_event_queue.queue) == NULL)
  {
    printf("No se pudo inicializar la cola de eventos\n");
    return 0;
  }

  if(!emit_event(&my_event_queue , 0))
  {
    printf("no anda esto master2\n");
    return 0;
  }
  if(!emit_event(&my_event_queue , 36))
  {
    printf("no anda esto master2\n");
    return 0;
  }
  if(!emit_event(&my_event_queue , 3))
  {
    printf("no anda esto master2\n");
    return 0;
  }
  printf("%d\n",get_event(&my_event_queue) );
  printf("%d\n",get_event(&my_event_queue) );
  printf("%d\n",get_event(&my_event_queue) );
  if(!emit_event(&my_event_queue , 1))
  {
    printf("no anda esto master2\n");
    return 0;
  }
  printf("%d\n",get_event(&my_event_queue) );
   if(!emit_event(&my_event_queue , 32))
  {
    printf("no anda esto master2\n");
    return 0;
  }
   if(!emit_event(&my_event_queue , 1))
  {
    printf("no anda esto master2\n");
    return 0;
  }
  printf("%d\n",get_event(&my_event_queue) );
  printf("%d\n",get_event(&my_event_queue) );


  destroy_queue(&my_event_queue);

  return 0;
}*/







event_queue_t create_queue ( void )
{
  event_queue_t newQueue,*p2NQueue = &newQueue;
  event_t *temp = (event_t*) malloc (QUEUE_SIZE * sizeof(event_t));
  int i;
  if(temp == NULL)
  {
    event_queue_t error = {.queue = NULL};
    return error;
  }
  event_t *tempaux = temp;
  for(i=0; i < QUEUE_SIZE; i++)
  {
    tempaux++ -> ack = true;
  }
  p2NQueue->queue = temp; //la cola de eventos
  p2NQueue->p2get = temp;//los punteros para leer y escribir estan en el mismo lugar (cola vacia)
  p2NQueue->p2write = temp;

  return newQueue;
}




bool destroy_queue ( event_queue_t* queue )
{
  if( queue == NULL)
  {
    return false;
  }
  free (queue->queue);
  return true;
}

bool emit_event( event_queue_t* queue , uint16_t type )
{
  event_t *temp = queue -> p2write;
  bool retval;

  if(queue == NULL)
  {
    return false;
  }


  if(queue -> p2write -> ack == false)
  {
    return false;
  }
  else
  {
    queue -> p2write -> ack = false;
    queue -> p2write -> type = type;


    if ((queue -> p2write) == ((queue -> queue) + (QUEUE_SIZE-1)))
    {
      queue -> p2write = queue -> queue;

    }
    else
    {
      (queue -> p2write) ++;
    }
    temp -> p2NextEv = (struct EVENT*) queue -> p2write;

    return true;
  }
}

uint16_t get_event ( event_queue_t* queue  )
{
  uint16_t retval;
  if(queue_is_empty(queue))
  {
    return 0;
  }
  retval = queue -> p2get->type;
  queue -> p2get -> ack = true;
  queue -> p2get =(event_t*) queue -> p2get ->p2NextEv;
  return retval;
}


bool queue_is_empty( event_queue_t* queue )
{
  return (queue -> p2get == queue -> p2write && queue -> p2get -> ack == true );
}
