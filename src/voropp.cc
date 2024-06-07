#include "voropp.h"
#include "voropp/container.hh"
#include "voropp/wall.hh"

using namespace voro;

void* create_container(double x_min, double x_max, double y_min, double y_max,
    double z_min, double z_max,
    int n_x, int n_y, int n_z,
    bool xperiodic, bool yperiodic ,bool zperiodic,
    int init_mem)
{
    return new container(x_min, x_max, y_min, y_max, z_min, z_max,
        n_x, n_y, n_z, xperiodic, yperiodic, zperiodic, init_mem);
}

void delete_container(void *ptr)
{
    container *con = (container *)ptr;
    delete con;
}

bool container_point_inside(void *ptr, double x, double y, double z)
{
    container *con = (container *)ptr;
    return con->point_inside(x, y, z);
}

void container_put(void *ptr, int n, double x, double y, double z)
{
    container *con = (container *)ptr;
    con->put(n, x, y, z);
}

void container_add_wall(void *ptr, void *pWall)
{
    container *con = (container *)ptr;
    wall *w = (wall*)pWall;
    con->add_wall(w);
}

void* create_wall_plane(double nx, double ny, double nz, double dist)
{
    wall *w = new wall_plane(nx, ny, nz, dist);
    return w;
}

void delete_wall(void *ptr)
{
    wall *w = (wall*)ptr;
    delete w;
}

double array_sum(double* arr, size_t length) {
    double sum = 0.0;
    for (size_t i = 0; i < length; ++i) {
        sum += arr[i];
    }
    return sum;
}
