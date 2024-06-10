#include "voro_wrap.h"
#include "voropp/voro++.hh"
#include "buffer.h"

using namespace voro;

#define INIT_MEM 8

double *calculate_voronoi(double *data)
{
    // Output will go here
    Buffer buf;
    buf.push(0); // total number of doubles in buffer
    buf.push(0); // number of cells

    size_t pos = 0;
    double ax = data[pos++];
    double bx = data[pos++];
    double ay = data[pos++];
    double by = data[pos++];
    double az = data[pos++];
    double bz = data[pos++];

    pre_container pcon(ax, bx, ay, by, az, bz, false, false, false);
	wall_list wl;

    size_t nWallPlanes = (size_t)data[pos++];
    for (size_t i = 0; i < nWallPlanes; ++i)
    {
        double wnx = data[pos++];
        double wny = data[pos++];
        double wnz = data[pos++];
        double wd = data[pos++];
        wl.add_wall(new wall_plane(wnx, wny, wnz, wd));
    }

    size_t nPoints = (size_t)data[pos++];
    for (size_t i = 0; i < nPoints; ++i)
    {
        int id = (int)data[pos++];
        double x = data[pos++];
        double y = data[pos++];
        double z = data[pos++];
        pcon.put(id, x, y, z);
    }

    int nx, ny, nz;
    pcon.guess_optimal(nx, ny, nz);

    container con(ax, bx, ay, by, az, bz,
        nx, ny, nz, false, false, false,
        INIT_MEM);
    con.add_wall(wl);
    pcon.setup(con);

    // Retrieve result
    voronoicell c;
    std::vector<int> fv;
    size_t nParticles = 0;
    c_loop_all vl(con);
    if (vl.start())
    {
        do
        {
            if (!con.compute_cell(c, vl)) continue;
            ++nParticles;

            // Particle ID and coordinates
            int pid;
            double x, y, z, r;
            vl.pos(pid, x, y, z, r);
            buf.push((double)pid);
            buf.push(x);
            buf.push(y);
            buf.push(z);

            // Cell volume
            buf.push(c.volume());

            // Number of vertices, and each vertex (x, y, z)
            buf.push((double)c.p);
            for (size_t i = 0; i < c.p; ++i)
            {
                buf.push(c.pts[3*i] * 0.5);
                buf.push(c.pts[3*i+1] * 0.5);
                buf.push(c.pts[3*i+2] * 0.5);
            }

            // Number of faces
            buf.push((double)c.number_of_faces());

            // Retrieve face vertices
            c.face_vertices(fv);
            for (int ix = 0; ix < fv.size(); )
            {
                int nVertsInFace = fv[ix++];
                buf.push((double)nVertsInFace);
                if (nVertsInFace == 1) buf.push((double)fv[ix++]);
                else if (nVertsInFace > 1)
                {
                    int end = ix + nVertsInFace;
                    buf.push((double)fv[ix++]);
                    while (ix < end) buf.push((double)fv[ix++]);
                }
            }
        }
        while (vl.inc());
    }

    wl.deallocate();

    buf.set_at(1, (double)nParticles);
    buf.set_at(0, (double)buf.pos);
    return (double*)buf.buf;
}
