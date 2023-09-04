#include "drone.h"

void create_drone(struct drone* d, int id, double x, double y, double vx, double vy){
    struct position p;
    struct velocity v;
    time_t current_time = time(NULL);
    p.x = x;
    p.y = y;
    p.last_time = current_time;
    v.x = vx;
    v.y = vy;

    d->id = id;
    d->pos = p;
    d->vel = v;
}

struct velocity drone_velocity(struct drone *d){
    return d->vel;
}

void drone_update_position(struct drone *d){
    time_t current_time = time(NULL);
    double interval = difftime(current_time, d->pos.last_time);
    d->pos.last_time = current_time;
    d->pos.x += d->vel.x * interval;
    d->pos.y += d->vel.y * interval;
}

struct position drone_position(struct drone *d){
    drone_update_position(d);
    return d->pos;
}

void drone_set_velocity(struct drone *d, double vx, double vy){
    drone_update_position(d);
    d->vel.x = vx;
    d->vel.y = vy;
}

void drone_set_id(struct drone *d, int id){
    d->id = id;
}

void set_objective(struct drone *d, double x, double y){
    drone_update_position(d);

    double vector_x = x - d->pos.x;
    double vector_y = y - d->pos.y;

    double module_v = sqrt(pow(d->vel.x, 2) + pow(d->vel.y, 2));
    double module_new = sqrt(pow(vector_x, 2) + pow(vector_y, 2));
    double new_vx = (vector_x/module_new) * module_v;
    double new_vy = (vector_y/module_new) * module_v;

    drone_set_velocity(d, new_vx, new_vy);
}

bool arrived(struct drone *d, double x, double y){
    drone_update_position(d);
    double distance = sqrt(pow(d->pos.x - x, 2) + pow(d->pos.y - y, 2));
    return distance <= ERR_POSITION;
}


