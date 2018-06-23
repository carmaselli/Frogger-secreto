#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


typedef struct EVENT
{
  uint16_t type;
  bool ack; //ver que hacer con esto
  struct EVENT *p2NextEv;
}event_t;

typedef struct
{
  event_t *queue;
  event_t *p2get;
  event_t *p2write;
}event_queue_t;


event_queue_t create_queue ( void );
bool destroy_queue ( event_queue_t* queue );
bool emit_event( event_queue_t* queue , uint16_t type );
uint16_t get_event ( event_queue_t* queue  );
bool queue_is_empty( event_queue_t* queue );

#endif
