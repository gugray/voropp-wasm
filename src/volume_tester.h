#pragma once

struct VolumeTester
{
    voro::wall_list *wl = NULL;
    voro::container *con = NULL;

    VolumeTester(double *data);
    ~VolumeTester();
    bool IsPointInside(double x, double y, double z) const;
};
