/* This is the server code */
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "drone.h"

#define SERVER_PORT 8080  /* arbitrary, but client & server must agree*/
#define BUF_SIZE 4096  /* block transfer size */
#define QUEUE_SIZE 10
#define MAX_MESSAGE_SIZE 2048

#define MAX_DRONES 10

int main(int argc, char *argv[])
{ 
   int s, b, l, fd, sa, bytes, on = 1;
   char buf[BUF_SIZE];  /* buffer for outgoing file */
   struct sockaddr_in channel;  /* holds IP address */
   /* Build address structure to bind to socket. */
   memset(&channel, 0, sizeof(channel));
   /* zero channel */
   channel.sin_family = AF_INET;
   channel.sin_addr.s_addr = htonl(INADDR_ANY);
   channel.sin_port = htons(SERVER_PORT);
   /* Passive open. Wait for connection. */
   s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* create socket */
   if (s < 0) {printf("socket call failed"); exit(-1);}
   setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));
   
   b = bind(s, (struct sockaddr *) &channel, sizeof(channel));
   if (b < 0) {
      printf("bind failed");
      exit(-1);
   }
   
   l = listen(s, QUEUE_SIZE); /* specify queue size */
   if(l < 0) {
      printf("listen failed");
      exit(-1);
   }

   struct drone drones[MAX_DRONES];
   int drones_count = 0;

   bool mission = false;
   double mission_x, mission_y;

   /* Socket is now set up and bound. Wait for connection and process it. */
   printf("Server is running...\n");

   while (1) {
      sa = accept(s, 0, 0); /* block for connection request */
      if (sa < 0) {
         printf("accept failed");
         exit(-1);
      }

      read(sa, buf, BUF_SIZE); /* read file name from socket */
      char message_aux[MAX_MESSAGE_SIZE];
      if(strncmp(buf, "MISSION", 7) == 0){
         int success = sscanf(buf, "MISSION %lf %lf", &mission_x, &mission_y);
         
         if(success < 2){
            write(sa, "ERROR\n", strlen("ERROR\n")+1);
         }
         else{
            if(mission){
               write(sa, "BUSY\n", strlen("BUSY\n")+1);
            }
            else{
               mission = true;
               write(sa, "OK\n", strlen("OK\n")+1);
            }
         }
      }
      else if(strcmp(buf, "HELLO") == 0){
         int id = ++drones_count;
         struct drone d;
         create_drone(&d, id, 0.0, 0.0, 0.0, 0.0);

         drones[id] = d;
         char response[MAX_MESSAGE_SIZE];
         snprintf(response, sizeof(response), "SERVER ONLINE %d\n", id);
         write(sa, response, strlen(response)+1);
      }
      else if(strncmp(buf, "UPDATE", 6) == 0){
         int id;
         float x, y, vx, vy;
         int success = sscanf(buf, "UPDATE %d %f %f %f %f", &id, &x, &y, &vx, &vy);
         
         if(success < 5){
            write(sa, "ERROR\n", strlen("ERROR\n")+1);
         }
         if(id > drones_count || id <= 0){
            write(sa, "ERROR\n", strlen("ERROR\n")+1);
         }
         else if(mission){
            drones[id].pos.x = x;
            drones[id].pos.y = y;
            drone_set_velocity(&drones[id], vx, vy);
            char response[MAX_MESSAGE_SIZE];
            snprintf(response, sizeof(response), "MISSION %lf %lf\n", mission_x, mission_y);

            write(sa, response, strlen(response)+1);
         }
         else{
            drones[id].pos.x = x;
            drones[id].pos.y = y;
            drone_set_velocity(&drones[id], vx, vy);
            write(sa, "OK\n", strlen("OK\n")+1);
         }
      }
      else if(strncmp(buf, "ARRIVED", 7) == 0){
         
         write(sa, "OK\n", strlen("OK\n")+1);
      }
      else if(strncmp(buf, "DONE", 4) == 0){
         printf("DONE\n");
         write(sa, "OK\n", strlen("OK\n")+1);
         mission = false;
      }
      else{
         write(sa, "UNKNOWN COMMAND\n", strlen("UNKNOWN COMMAND\n")+1);
      }


      close(sa); /* close connection */
   }   
}

