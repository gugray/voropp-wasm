#include "voropp/voro++.hh"
#include "volume_tester.h"

using namespace voro;

#define BLOCK_SIZE 7
#define INIT_MEM 8

VolumeTester::VolumeTester(double *data)
{
    this->wl = new wall_list();

    size_t pos = 0;
    double ax = data[pos++];
    double bx = data[pos++];
    double ay = data[pos++];
    double by = data[pos++];
    double az = data[pos++];
    double bz = data[pos++];

    this->con = new container(ax, bx, ay, by, az, bz,
        BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE,
        false, false, false, INIT_MEM);

    size_t nWallPlanes = (size_t)data[pos++];
    for (size_t i = 0; i < nWallPlanes; ++i)
    {
        double wnx = data[pos++];
        double wny = data[pos++];
        double wnz = data[pos++];
        double wd = data[pos++];
        wall_plane *wp = new wall_plane(wnx, wny, wnz, wd, -1 - i);
        this->wl->add_wall(wp);
    }
    this->con->add_wall(*this->wl);
}

VolumeTester::~VolumeTester()
{
    delete this->con;
    this->wl->deallocate();
    delete this->wl;
}

bool VolumeTester::IsPointInside(double x, double y, double z) const
{
    return this->con->point_inside(x, y, z);
}
