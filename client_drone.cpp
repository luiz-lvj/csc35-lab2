/* This page contains a client program that can request a file from the server program
* on the next page. The server responds by sending the whole file.
*/


#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "drone.h"
#include "messages.h"

#define TIME_MISSION 10


int main(int argc, char **argv)
{
  if (argc != 5) {
    printf("Usage: client x y vx vy\n");
    exit(-1);
  }

  char host[] = "localhost";  /* host to use if none supplied */
  char file[] = "out.txt";  /* file to get from server */
  int c, s, bytes;
  //char buf[BUFSIZE];  /* buffer for incoming file */
  struct hostent *h;  /* info about server */
  struct sockaddr_in channel; /* holds IP address */

  struct drone drone;

  create_drone(&drone, 0, atof(argv[1]), atof(argv[2]), atof(argv[3]), atof(argv[4]));
  h = gethostbyname(host); /* look up host’s IP address */

  if (!h) {
    printf("gethostbyname failed to locate %s", host);
    exit(-1);
  }
  /* start message*/

  bool logged = false;
  bool mission = false;
  double mission_x = 0;
  double mission_y = 0;

  bool solving = false;
  
  while(1){
    if(!logged){
      if(send_message(HELLO, &drone, &mission, &mission_x, &mission_y, c, s, bytes, channel, h)){
        logged = true;
      }
    }
    else if(!mission){
      if(!send_message(UPDATE, &drone, &mission, &mission_x, &mission_y, c, s, bytes, channel, h)){
        logged = false;
      } else {
        sleep(2);
      }
    }
    else {
      if(arrived(&drone, mission_x, mission_y)){
        double vx = drone_velocity(&drone).x;
        double vy = drone_velocity(&drone).y;

        drone_set_velocity(&drone, 0, 0);
        if(!send_message(ARRIVED, &drone, &mission, &mission_x, &mission_y, c, s, bytes, channel, h)){
          logged = false;
        }
        else{
          sleep(TIME_MISSION); // mission being executed
          if(!send_message(DONE, &drone, &mission, &mission_x, &mission_y, c, s, bytes, channel, h)){
            logged = false;
          }

          mission = false;
        }
        drone_set_velocity(&drone, vx, vy);
      }
      else{
        printf("drone position: %f, %f\n", drone_position(&drone).x, drone_position(&drone).y);
      }

    }
    //
  }
}
