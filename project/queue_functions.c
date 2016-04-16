
#include "main.h"
#include <math.h>
#include "elev.h"
#include "communication.h"


int add_to_queue(elev_button_type_t type, int  floor){
  if(type == BUTTON_CALL_UP && !queue[floor][0]){
    queue[floor][0] = 1;
    return 1; 
  }
  else if(type == BUTTON_CALL_DOWN && !queue[floor+2][0]){
    queue[floor+2][0] = 1;
    return 1;
  }
  else
    return 0;
}
//add_to_queue returns positive -> new order from outside
int cost_function(elev_button_type_t type, int floor){
  int direction;
  if(type==BUTTON_CALL_UP)
    direction = DIRN_UP;
  else
    direction = DIRN_DOWN;
  printf("DIRN: %d, FLOOR: %d\n", direction, floor);

  int delta_elev_free=5;
  int delta_elev_running=5; //Larger than possible delta FLOOR DIFFERANCE
  int delta_elev_top=5;
  int delta_elev_bottom=5;
  int client_running=-1,client_bottom=-1,client_top=-1,client_free = -1;//Unavilable

  //Elevator is available
  for(int i=0;i<clients;i++){
    if(elev_client[i].direction == DIRN_STOP){
      if(abs(elev_client[i].floor_current-floor)<delta_elev_free){
        delta_elev_free=abs(elev_client[i].floor_current-floor);
        client_free = i;
      }
    } 
  }


  if(floor==3){
   for(int i=0;i<clients;i++){
      if(elev_client[i].direction == DIRN_UP && elev_client[i].queue[floor]==1){
        if(abs(elev_client[i].floor_current-floor)<delta_elev_top){
          delta_elev_top=abs(elev_client[i].floor_current-floor);
          client_top = i;
        }   
      }
    }
    if(client_top==-1){
      for(int i=0;i<clients;i++){
          if(elev_client[i].direction == DIRN_UP){
            if(abs(elev_client[i].floor_current-floor)<delta_elev_top){
              delta_elev_top=abs(elev_client[i].floor_current-floor);
              client_top = i;
            } 
          }
      }
    }
  }

  else if(floor==0){
   for(int i=0;i<clients;i++){
      if(elev_client[i].direction == DIRN_DOWN && elev_client[i].queue[floor]==1){
        if(abs(elev_client[i].floor_current-floor)<delta_elev_bottom){
          delta_elev_bottom=abs(elev_client[i].floor_current-floor);
          client_bottom =i;
        }   
      }
    }
    if(client_bottom==-1){
      for(int i=0;i<clients;i++){
        if(elev_client[i].direction == DIRN_DOWN){
          if(abs(elev_client[i].floor_current-floor)<delta_elev_bottom){
            delta_elev_bottom=abs(elev_client[i].floor_current-floor);
            client_bottom =i;
          }   
        }
      }
    }
  }

  /*
  //Elevator runs in the ordrered direction, and is stopping at the same floor
    for(int i=0;i<clients;i++){
      if(direction == DIRN_UP && elev_client[i].direction == direction && elev_client[i].queue[floor]==1){
        if(abs(elev_client[i].floor_current-floor)<delta){
          delta=abs(elev_client[i].floor_current-floor);
          client =i;
        }   
      }
    }
    if(client!=-1){
      return client;
    }

  */

  //Elevator runs in the ordererd direction, but is not stopping at the same floor
  for(int i=0;i<clients;i++){
    if(direction == DIRN_UP && elev_client[i].direction == DIRN_UP && elev_client[i].floor_current<=floor){
      if(abs(elev_client[i].floor_current-floor)<delta_elev_running){
        delta_elev_running=abs(elev_client[i].floor_current-floor);
        client_running =i;
      }
    }
  }


  //Elevator runs in the ordererd direction, but is not stopping at the same floor
  for(int i=0;i<clients;i++){
    if(direction == DIRN_DOWN && elev_client[i].direction == DIRN_DOWN && elev_client[i].floor_current>=floor){
      if(abs(elev_client[i].floor_current-floor)<delta_elev_running){
        delta_elev_running=abs(elev_client[i].floor_current-floor);
        client_running =i;
      }
    }
  }


  //At this point I know that all elevators are running in the wrong direction
  // -> I assign client 1
  printf("Client top: %d \t Client Bottom: %d \t Client Running: %d \t Client free: %d\n", client_top, client_bottom, client_running, client_free);
  if((client_top == -1) && (client_free == -1) && (client_bottom == -1) && (client_running == -1))
    return 0;

  switch (floor){
    case 0:
      if(delta_elev_bottom<delta_elev_free){
        return client_bottom;
      }
      else 
        return client_free;
      break;
    case 3:
      if(delta_elev_top<delta_elev_free){
        return client_top;
      }
      else
       return client_free;
      break;
    default:
      if(delta_elev_running<delta_elev_free){
        return client_running;
      }
      else
        return client_free;
    break;
  } 
}
void reallocate_orders(int number_of_clients){
  if(number_of_clients==2)
  {
    if(number_of_clients == 1){
      for(int i = 0;i<6;i++){
        if(queue[i][0] == 1){
          queue[i][1] = 0;
        }
      }
    }
    else if(number_of_clients == 2){
      for(int i = 0;i<3;i++){
        if(queue[i][0] == 1)
          queue[i][1] = 0;
      }
      for(int i = 3;i<6;i++){
        if(queue[i][0] == 1){
          queue[i][1] = 1;
        }
      }
    }
  }
  Message message_send;
  
  char server_message[BUFSIZE];
    for(int i = 0;i<6;i++){
      message_send.orders[i][0] = queue[i][0];
      message_send.orders[i][1] = queue[i][1];
    }

  serialization(ORDER_UPDATE, message_send, server_message);

  for(int i = 0;i<clients;i++){
      write(client_sockets[i], server_message, strlen(server_message));  
  }
}

