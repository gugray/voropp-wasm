#include "voro_wrap.h"
#include "voropp/pre_container.hh"
#include "voropp/container.hh"
#include "voropp/wall.hh"
#include "voropp/c_loops.hh"
#include "buffer.h"

using namespace voro;

// void* create_pre_container(double x_min, double x_max,
//     double y_min, double y_max,
//     double z_min, double z_max)
// {
//     return new pre_container(x_min, x_max, y_min, y_max, z_min, z_max, false, false, false);
// }

// void delete_pre_container(void *ptr)
// {
//     pre_container *pcon = (pre_container*)ptr;
//     delete pcon;
// }

void* create_container(double x_min, double x_max,
    double y_min, double y_max,
    double z_min, double z_max,
    int n_x, int n_y, int n_z, int init_mem)
{
    return new container(x_min, x_max, y_min, y_max, z_min, z_max,
        n_x, n_y, n_z, false, false, false, init_mem);
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

double* container_compute(void *ptr)
{
    Buffer buf;
    buf.push(0); // total number of doubles in buffer
    buf.push(0); // number of particles
    container *con = (container *)ptr;
    c_loop_all vl(*con);
    voronoicell c;
    std::vector<int> fv; // Receives face vertices
    size_t nParticles = 0;
    bool gotWork = vl.start();
    while (gotWork)
    {
        gotWork = vl.inc();
        if (!con->compute_cell(c, vl)) continue;
        ++nParticles;

        // Particle ID and coordinates
        int pid;
        double x, y, z, r;
        vl.pos(pid, x, y, z, r);
        buf.push((double)pid);
        buf.push(x);
        buf.push(y);
        buf.push(z);

        // Number of vertices, and each vertex (x, y, z)
        buf.push((double)c.p);
        for (size_t i = 0; i < c.p; ++i)
        {
            buf.push(c.pts[3*i]);
            buf.push(c.pts[3*i+1]);
            buf.push(c.pts[3*i+2]);
        }

        // This will be face count; filled in later
        size_t faceCountPos = buf.pos;
        size_t faceCount = 0;
        buf.push(0);
        // Retrieve and interpret face vertices
        c.face_vertices(fv);
        for (size_t i = 0; i < fv.size(); ++i)
        {
            size_t nVertsInFace = fv[i++];
            if (nVertsInFace < 3) continue;
            ++faceCount;
            buf.push((double)nVertsInFace);
            for (size_t j = 0; j < nVertsInFace; ++j, ++i)
                buf.push((double)fv[i]);
        }
        // Backfill face count into buffer
        buf.set_at(faceCountPos, (double)faceCount);
    }

    buf.set_at(1, (double)nParticles);
    buf.set_at(0, (double)buf.pos);
    return (double*)buf.buf;
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
