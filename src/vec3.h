#pragma once
#include <cmath>

struct Vec3
{
    double x, y, z;
    Vec3()
    {
        this->x = this->y = this->z = 0;
    }
    Vec3(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    inline Vec3& operator=(const Vec3 &rhs)
    {
        this->x = rhs.x;
        this->y = rhs.y;
        this->z = rhs.z;
        return *this;
    }
    inline double length() const
    {
        return sqrt(x*x + y*y + z*z);
    }
    inline void setLength(double length)
    {
        double f = length / this->length();
        this->x *= f;
        this->y *= f;
        this->z *= f;
    }
    inline Vec3& add(const Vec3 &rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        return *this;
    }
    inline Vec3& mul(double scalar)
    {
        this->x *= scalar;
        this->y *= scalar;
        this->z *= scalar;
        return *this;
    }
};

// Calculates distance of plane defined by (v1, v2, v3) triangle from point p.
inline double distPlanePt(const Vec3& v1, const Vec3& v2, const Vec3& v3, const Vec3& p)
{
    double a = (v2.y - v1.y) * (v3.z - v1.z) - (v2.z - v1.z) * (v3.y - v1.y);
    double b = (v2.z - v1.z) * (v3.x - v1.x) - (v2.x - v1.x) * (v3.z - v1.z);
    double c = (v2.x - v1.x) * (v3.y - v1.y) - (v2.y - v1.y) * (v3.x - v1.x);
    double d = abs(a * (p.x - v1.x) + b * (p.y - v1.y) + c * (p.z - v1.z));
    return d / sqrt(a * a + b * b + c * c);
}

// Calculates distance of plane defined by (v1, v2, v3) triangle from the origin.
inline double distPlaneOrigin(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
    double a = (v2.y - v1.y) * (v3.z - v1.z) - (v2.z - v1.z) * (v3.y - v1.y);
    double b = (v2.z - v1.z) * (v3.x - v1.x) - (v2.x - v1.x) * (v3.z - v1.z);
    double c = (v2.x - v1.x) * (v3.y - v1.y) - (v2.y - v1.y) * (v3.x - v1.x);
    double d = abs(a * -v1.x + b * -v1.y + c * -v1.z);
    return d / sqrt(a * a + b * b + c * c);
}
