#ifndef DRONE_H
#define DRONE_H
#include <time.h>
#include <math.h>
#include <stdio.h>

#define ERR_POSITION 1


struct velocity{
    double x;
    double y;
};

struct position{
    double x;
    double y;
    time_t last_time;
};

struct drone{
    int id;
    struct position pos;
    struct velocity vel;
};

void create_drone(struct drone* drone, int id, double x, double y, double vx, double vy);
struct position drone_position(struct drone *d);
struct velocity drone_velocity(struct drone *d);
void drone_update_position(struct drone *d);
void drone_set_velocity(struct drone *d, double vx, double vy);
void drone_set_id(struct drone *d, int id);
void set_objective(struct drone *d, double x, double y);
bool arrived(struct drone *d, double x, double y);



#endif // DRONE_H