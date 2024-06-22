#include "voro_wrap.h"
#include "voropp/voro++.hh"
#include "buffer.h"
#include "vec3.h"
#include "volume_tester.h"

using namespace voro;

#define INIT_MEM 8

bool inset_faces(
    voronoicell_neighbor &c,
    const std::vector<Vec3> &verts,
    const std::vector<int> &faceFirstTriIxs,
    std::vector<double> &nor,
    std::vector<int> &neigh,
    double inset);

double *calculate_voronoi(double *data, double inset)
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
        wl.add_wall(new wall_plane(wnx, wny, wnz, wd, -1 - i));
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
    voronoicell_neighbor c, c2;
    std::vector<Vec3> verts;
    std::vector<int> fv;
    std::vector<int> faceFirstTriIxs; // Indexes of first 3 vertices from each face; 3 items per face
    std::vector<double> nor; // "Local" variable of inset_faces function; reused to save allocations
    std::vector<int> neigh; // "Local" variable of inset_faces function; reused to save allocations
    size_t nParticles = 0;
    c_loop_all vl(con);
    if (vl.start())
    {
        do
        {
            if (!con.compute_cell(c, vl)) continue;
            ++nParticles;
            verts.clear();
            faceFirstTriIxs.clear();

            // Particle ID and coordinates
            int pid;
            Vec3 part;
            double r;
            vl.pos(pid, part.x, part.y, part.z, r);
            buf.push((double)pid);
            buf.push(part.x);
            buf.push(part.y);
            buf.push(part.z);

            // Cell volume
            buf.push(c.volume());

            // Number of vertices, and each vertex (x, y, z)
            buf.push((double)c.p);
            for (size_t i = 0; i < c.p; ++i)
            {
                Vec3 v(c.pts[3*i] * 0.5, c.pts[3*i+1] * 0.5, c.pts[3*i+2] * 0.5);
                verts.push_back(v);
                buf.push(v.x);
                buf.push(v.y);
                buf.push(v.z);
            }

            // Number of faces
            buf.push((double)c.number_of_faces());

            // Retrieve face vertices
            c.face_vertices(fv);
            for (int ix = 0; ix < fv.size(); )
            {
                int nVertsInFace = fv[ix++];
                buf.push((double)nVertsInFace);
                if (nVertsInFace < 3)
                {
                    for (int k = 0; k < 3; ++k)
                        faceFirstTriIxs.push_back(-1);
                }
                else
                {
                    for (int k = 0; k < 3; ++k)
                        faceFirstTriIxs.push_back(fv[ix+k]);
                }
                int end = ix + nVertsInFace;
                while (ix < end) buf.push((double)fv[ix++]);
            }

            // Perfom insetting on copy, if requested
            bool gotRealInset = false;
            if (inset != 0)
            {
                c2 = c;
                gotRealInset = inset_faces(c2, verts, faceFirstTriIxs, nor, neigh, inset);
            }
            
            // After inset: number of vertices, and each vertex
            // If 0, it means cell disappeared through insetting, or no insetting was requested
            if (!gotRealInset)
            {
                buf.push(0);
                continue;
            }

            // Number of vertices, and each vertex (x, y, z)
            buf.push((double)c2.p);
            for (size_t i = 0; i < c2.p; ++i)
            {
                buf.push(c2.pts[3*i] * 0.5);
                buf.push(c2.pts[3*i+1] * 0.5);
                buf.push(c2.pts[3*i+2] * 0.5);
            }

            // Number of faces
            buf.push((double)c2.number_of_faces());

            // Retrieve face vertices
            c2.face_vertices(fv);
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

bool inset_faces(
    voronoicell_neighbor &c,
    const std::vector<Vec3> &verts,
    const std::vector<int> &faceFirstTriIxs,
    std::vector<double> &nor,
    std::vector<int> &neigh,
    double inset)
{
    bool cellRemaining = true;

    // Get cell's normal vectors
    c.normals(nor);
    c.neighbors(neigh);

    // Plane with each of them
    size_t nFaces = c.number_of_faces();
    for (size_t i = 0; cellRemaining && i < nFaces; ++i)
    {
        // Face's neighbor is a wall: not insetting this face
        if (neigh[i] < 0) continue;

        // Face's normal vector; particle
        Vec3 n(nor[i*3], nor[i*3+1], nor[i*3+2]);

        // Distance of face's plane from particle
        Vec3 v1 = verts[faceFirstTriIxs[i*3]];
        Vec3 v2 = verts[faceFirstTriIxs[i*3+1]];
        Vec3 v3 = verts[faceFirstTriIxs[i*3+2]];
        double d = distPlaneOrigin(v1, v2, v3);

        // Shave off the cell
        double len = (d - inset) * 2;
        if (len >= 0) cellRemaining &= c.plane(n.x, n.y, n.z, len);
        else
        {
            n.mul(-1);
            cellRemaining &= c.plane(n.x, n.y, n.z, -len);
        }
    }

    return cellRemaining;
}

void *create_volume_tester(double *data)
{
    return new VolumeTester(data);
}

void delete_volume_tester(void *ptr)
{
    VolumeTester *vt = (VolumeTester*)ptr;
    delete vt;
}

bool is_point_inside(void *ptr, double x, double y, double z)
{
    VolumeTester *vt = (VolumeTester*)ptr;
    return vt->IsPointInside(x, y, z);
}


