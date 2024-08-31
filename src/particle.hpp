#pragma once

#include <cstddef>

#include "vectors.hpp"
#include "raylib.h"

namespace Particle {

    class Particle {

    public:
        Vectors::Vec3 position;
        Vectors::Vec3 velocity;

        Vectors::Vec3 acceleration;
        Vectors::Vec3 prev_acceleration;

        float radius;
        float mass;

        Color color1 = BLUE;
        Color color2 = RED;

        void apply_gravity(const Particle &other, float G);
        void update(float delta_time);
        void collision(Particle &other);
        void draw(Mesh mesh, Material material);

        std::size_t id;

    };

}
