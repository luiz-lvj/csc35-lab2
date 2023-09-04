#ifndef MESSAGES_H
#define MESSAGES_H

#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "drone.h"

#define MAX_MESSAGE_SIZE 2048
#define SERVER_PORT 8080 /* arbitrary, but client & server must agree */
#define BUFSIZE 4096  /* block transfer size */

enum message_type{
    HELLO, 
    UPDATE,
    MISSION,
    ARRIVED,
    DONE
};

bool send_message(enum message_type msg, struct drone* drone, bool* mission, double * mission_x, double * mission_y, int c, int s, int bytes, struct sockaddr_in channel, struct hostent *h);

#endif // MESSAGES_H