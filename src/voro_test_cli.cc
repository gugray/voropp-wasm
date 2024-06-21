#include "voropp/voro++.hh"
#include "vec3.h"
#include <cstring>

using namespace voro;

double ax = -1;
double bx = 1;
double ay = -1;
double by = 1;
double az = -1;
double bz = 1;


double wla = 1;
double wlb = 0.4;
double wld = 0.4;

int init_mem = 8;
char *format = "ID %i POS (%x,%y,%z) VERTS %w %p EDGES %g FACES %s %t";

int main(int argc,char **argv)
{
    pre_container pcon(ax, bx, ay, by, az, bz, false, false, false);
	wall_list wl;

    wl.add_wall(new wall_plane(wla, wlb, 0, wld, -1));
    wl.add_wall(new wall_plane(wla, -wlb, 0, wld, -2));
    wl.add_wall(new wall_plane(-wla, -wlb, 0, wld, -3));
    wl.add_wall(new wall_plane(-wla, wlb, 0, wld, -4));
    wl.add_wall(new wall_plane(0, wlb, wla, wld, -5));
    wl.add_wall(new wall_plane(0, -wlb, wla, wld, -6));
    wl.add_wall(new wall_plane(0, -wlb, -wla, wld, -7));
    wl.add_wall(new wall_plane(0, wlb, -wla, wld, -8));

    pcon.put(0, -0.15, 0.00, 0.87);
    pcon.put(1, -0.19, -0.23, 0.15);
    pcon.put(2, -0.39, 0.01, -0.51);
    pcon.put(3, 0.11, -0.00, -0.98);
    pcon.put(4, 0.34, 0.08, 0.05);

    int nx, ny, nz;
    pcon.guess_optimal(nx, ny, nz);

    container con(ax, bx, ay, by, az, bz,
        nx, ny, nz, false, false, false,
        init_mem);
    con.add_wall(wl);

    pcon.setup(con);

    c_loop_all vl(con);

    FILE *fp = safe_fopen("voro_test_out.txt", "w");
    
    voronoicell_neighbor c, c2;
    std::vector<int> fv;
    std::vector<double> nor;
    std::vector<Vec3> verts;
    std::vector<int> neighbors;
    std::vector<int> faceFirstTriIxs; // Indexes of first 3 vertices from each face; 3 items per face
    int pid;
    double x, y, z, r;
    if (vl.start())
    {
        do
        {
            if (!con.compute_cell(c, vl)) continue;
            verts.clear();
            faceFirstTriIxs.clear();
            vl.pos(pid, x, y, z, r);
            fprintf(fp, "ID %d POS (%g,%g,%g) VERTS %d", pid, x, y, z, c.p);
            for (size_t i = 0; i < c.p; ++i)
            {
                Vec3 v(c.pts[3*i] * 0.5, c.pts[3*i+1] * 0.5, c.pts[3*i+2] * 0.5);
                verts.push_back(v);
                fprintf(fp, " (%g,%g,%g)", v.x, v.y, v.z);
            }
            fprintf(fp, " EDGES %d", c.number_of_edges());
            fprintf(fp, " FACES %d", c.number_of_faces());
            c.face_vertices(fv);
            for (int ix = 0; ix < fv.size(); )
            {
                int nVertsInFace = fv[ix++];
                if (nVertsInFace < 3)
                {
                    faceFirstTriIxs.push_back(-1);
                    faceFirstTriIxs.push_back(-1);
                    faceFirstTriIxs.push_back(-1);

                }
                else
                {
                    faceFirstTriIxs.push_back(fv[ix]);
                    faceFirstTriIxs.push_back(fv[ix+1]);
                    faceFirstTriIxs.push_back(fv[ix+2]);

                }
                if (nVertsInFace <= 1)
                {
                    if (nVertsInFace==1) fprintf(fp," (%d)", fv[ix++]);
                    else fputs(" ()", fp);
                }
                else
                {
                    int end = ix + nVertsInFace;
                    fprintf(fp, " (%d", fv[ix++]);
                    while (ix < end) fprintf(fp, ",%d", fv[ix++]);
                    fputs(")", fp);
                }
            }

            // Get cell's normal vectors
            c2 = c;
            c2.normals(nor);
            c2.neighbors(neighbors);
            size_t kk = neighbors.size();

            // Plane with each of them
            size_t nFaces = c.number_of_faces();
            for (size_t i = 0; i < nFaces; ++i)
            {
                // Face's normal vector; particle
                Vec3 n(nor[i*3], nor[i*3+1], nor[i*3+2]);
                Vec3 p(x, y, z);
                // Distance of face's plane from particle
                Vec3 v1 = verts[faceFirstTriIxs[i*3]];
                Vec3 v2 = verts[faceFirstTriIxs[i*3+1]];
                Vec3 v3 = verts[faceFirstTriIxs[i*3+2]];
                double d = distPlanePt(v1, v2, v3, p);
                // Shave off the cell
                c2.plane(n.x, n.y, n.z, d - 0.02);
            }


            fprintf(fp, "\n");
        }
        while (vl.inc());
    }

    wl.deallocate();
	fclose(fp);
}
