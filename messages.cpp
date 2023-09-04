#include "messages.h"


bool send_message(enum message_type msg, struct drone* drone, bool* mission, double * mission_x, double * mission_y, int c, int s, int bytes, struct sockaddr_in channel, struct hostent *h){
  char buf[BUFSIZE];  /* buffer for incoming file */
  /* info about server *//* holds IP address */
  char message[MAX_MESSAGE_SIZE] = "A";  /* file to get from server */

    s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(s <0){
        printf("socket call failed"); exit(-1);
    }
    memset(&channel, 0, sizeof(channel));
    channel.sin_family= AF_INET;
    memcpy(&channel.sin_addr.s_addr, h->h_addr, h->h_length);
    channel.sin_port= htons(SERVER_PORT);
    c = connect(s, (struct sockaddr *) &channel, sizeof(channel));
    if (c < 0) {
        printf("connect failed");
        exit(-1);
    }

    if(msg == HELLO){
        sprintf(message, "HELLO");
    }
    else if(msg == UPDATE){
        sprintf(message, "UPDATE %d %f %f %f %f", drone->id, drone_position(drone).x, drone_position(drone).y, drone_velocity(drone).x, drone_velocity(drone).y);
    }
    else if(msg == ARRIVED){
        sprintf(message, "ARRIVED %d", drone->id);
    }
    else if(msg == DONE){
        sprintf(message, "DONE %d", drone->id);
    }
    else {
        return false;
    }

  /* Connection is now established. Send file name including 0 byte at end. */
    write(s, message, strlen(message)+1);

    /* Go get the file and write it to standard output.*/
    while (1) {
        bytes = read(s, buf, BUFSIZE); /* read from socket */
        if (bytes <= 0) break; /* check for end of file */

        if(msg == HELLO){
            int drone_id;
            int success = sscanf(buf, "SERVER ONLINE %d\n", &drone_id);

            if(success < 1){
                return false;
            }
            drone_set_id(drone, drone_id);
        }
        else if(msg == UPDATE){
            double mission_server_x, mission_server_y;

            int success_data_mission = sscanf(buf, "MISSION %lf %lf\n", &mission_server_x, &mission_server_y);
            if(success_data_mission == 2){
                *mission = true;
                *mission_x = mission_server_x;
                *mission_y = mission_server_y;
                set_objective(drone, mission_server_x, mission_server_y);
            } else{
                if(strcmp(buf, "OK\n") != 0){
                    return false;
                }
            } 
        }
        else if(msg == ARRIVED) {
            if(strcmp(buf, "OK\n") != 0){
                return false;
            }
        }
        else if(msg == DONE) {
            if(strcmp(buf, "OK\n") != 0){
                return false;
            }
        }
        write(1, "DRONE: ", strlen("DRONE: ")+1);  /* write to standard output */
        write(1, message, strlen(message)+1);  /* write to standard output */
        write(1, "\nSERVER: ", strlen("\nSERVER: ")+1);
        write(1, buf, bytes);  /* write to standard output */
    }
    
    return strcmp(buf, "SERVER ONLINE\n") || strcmp(buf, "OK\n");
}