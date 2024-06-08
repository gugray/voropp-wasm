#include <stdlib.h>
#include <math.h>

extern "C"
{

// void* create_pre_container(double x_min, double x_max,
//     double y_min, double y_max,
//     double z_min, double z_max);

// void delete_pre_container(void *ptr);

void* create_container(double x_min, double x_max,
    double y_min, double y_max,
    double z_min, double z_max,
    int n_x, int n_y, int n_z, int init_mem);

void delete_container(void *ptr);

bool container_point_inside(void *ptr, double x, double y, double z);

void container_put(void *ptr, int n, double x, double y, double z);

void container_add_wall(void *ptr, void *pWall);

double* container_compute(void *ptr);

void delete_wall(void *ptr);

void* create_wall_plane(double nx, double ny, double nz, double dist);

}