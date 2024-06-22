#pragma once

#include <stdlib.h>
#include <math.h>

extern "C"
{

double *calculate_voronoi(double *data, double inset);

void *create_volume_tester(double *data);
void delete_volume_tester(void *ptr);
bool is_point_inside(void *ptr, double x, double y, double z);

}
