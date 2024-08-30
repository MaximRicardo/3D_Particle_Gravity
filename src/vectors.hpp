#pragma once

#include "raylib.h"

namespace Vectors {

    class Vec3 {

    public:
        float x, y, z;

        Vec3 operator+(const Vec3 &v) const;
        Vec3 operator-(const Vec3 &v) const;
        Vec3 operator*(const float x) const;
        Vec3 operator/(const float x) const;

        float dot(const Vec3 &v) const;

        float length() const;
        float magnitude() const;
        Vec3 normalized() const;

        float dist(const Vec3 &v) const;

        Vec3 lerp(const Vec3 &v, float t) const;

        operator Vector3() const;

    };

}
