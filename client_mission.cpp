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
#define SERVER_PORT 8080 /* arbitrary, but client & server must agree */
#define BUFSIZE 4096  /* block transfer size */

#define MAX_MESSAGE_SIZE 2048


int main(int argc, char **argv)
{
  char host[] = "localhost"; 
  int c, s, bytes;
  char buf[BUFSIZE];  /* buffer for incoming file */
  struct hostent *h;  /* info about server */
  struct sockaddr_in channel; /* holds IP address */
  if (argc != 3) {
    printf("Usage: client target_x target_y");
    exit(-1);
  }
  h = gethostbyname(host); /* look up host’s IP address */
  if(!h) {
    printf("gethostbyname failed to locate %s", host);
    exit(-1);
  }
  
  s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s <0) {
    printf("socket call failed");
    exit(-1);
  }
  memset(&channel, 0, sizeof(channel));
  channel.sin_family= AF_INET;
  memcpy(&channel.sin_addr.s_addr, h->h_addr, h->h_length);
  channel.sin_port= htons(SERVER_PORT);
  c = connect(s, (struct sockaddr *) &channel, sizeof(channel));
  if (c < 0) {printf("connect failed"); exit(-1);} 
  /* Connection is now established. Send file name including 0 byte at end. */

  char message[MAX_MESSAGE_SIZE];
  sprintf(message, "MISSION %s %s", argv[1], argv[2]);

  write(s, message, strlen(message)+1);
  
  /* Go get the file and write it to standard output.*/
  while (1) {
     bytes = read(s, buf, BUFSIZE); /* read from socket */
     if (bytes <= 0) exit(0); /* check for end of file */
     write(1, buf, bytes);  /* write to standard output */
  }
}

