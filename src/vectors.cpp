#include <cmath>

#include "vectors.hpp"

namespace Vectors {

    Vec3 Vec3::operator+(const Vec3 &v) const {
        return {x+v.x, y+v.y, z+v.z};
    }

    Vec3 Vec3::operator-(const Vec3 &v) const {
        return {x-v.x, y-v.y, z-v.z};
    }

    Vec3 Vec3::operator*(const float x) const {
        return {this->x*x, this->y*x, this->z*x};
    }

    Vec3 Vec3::operator/(const float x) const {
        return {this->x/x, this->y/x, this->z/x};
    }

    float Vec3::dot(const Vec3 &v) const {
        return x*v.x + y*v.y + z*v.z;
    }

    float Vec3::length() const {
        return std::sqrt(this->dot(*this));
    }

    float Vec3::magnitude() const {
        return length();
    }

    Vec3 Vec3::normalized() const {
        return *this/this->length();
    }
        
    float Vec3::dist(const Vec3 &v) const {
        float diff_x = v.x - x;
        float diff_y = v.y - y;
        float diff_z = v.z - z;
        return std::sqrt(diff_x*diff_x + diff_y*diff_y + diff_z*diff_z);
    }
    
    Vec3 Vec3::lerp(const Vec3 &v, float t) const {

        float new_x = x + (v.x - x)*t;
        float new_y = y + (v.y - y)*t;
        float new_z = z + (v.z - z)*t;
        return {new_x, new_y, new_z};

    }

    Vec3::operator Vector3() const {
        return {x, y, z};
    }

}
