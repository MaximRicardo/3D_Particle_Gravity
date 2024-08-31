#include <cmath>
#include <cstdio>

#include "raylib.h"
#include "raymath.h"
#include "vectors.hpp"
#include "particle.hpp"

namespace Particle {

    void Particle::apply_gravity(const Particle &other, float G) {

        if (position.dist(other.position) == 0.f) return;

        //a = MG/r²
        float a = other.mass*G/std::pow(position.dist(other.position), 2.f);
        Vectors::Vec3 dir = (other.position - position).normalized();
        acceleration = acceleration + dir*a;

    }
    
    void Particle::update(float delta_time) {

        velocity = velocity + acceleration*delta_time;
        position = position + velocity*delta_time;

        prev_acceleration = acceleration;
        acceleration = {0.f, 0.f, 0.f};

    }

    void Particle::collision(Particle &other) {

        float dist = position.dist(other.position);

        //If the distance is greater than the combined radii, then there cannot physically be a collision
        if (dist >= radius+other.radius) return;

        float move_back_amount = (radius+other.radius)-dist;  //Move just enough to get outside of the radius of the other particle
        Vectors::Vec3 move_back_dir = (position-other.position);  //Move away from the other particle
        if (move_back_dir.length() == 0.f) return;  //For some reason, this vector is sometimes all zeroes
        move_back_dir = move_back_dir.normalized();

        position = position + move_back_dir*move_back_amount;

        //Remove the part of the velocity that goes toward the collision. This simulates the impact being absorbed
        Vectors::Vec3 n = move_back_dir * -1.f;
        float this_d = velocity.dot(n);
        if (this_d > 0.f) {
            velocity = velocity - n*this_d;

            other.velocity = other.velocity + (n*this_d/1.1f)*mass/other.mass;
        }

    }

    void Particle::draw(Mesh mesh, Material material) {

        //Lerp between blue and red depending on acceleration
        
        Color color1 = BLUE;
        Color color2 = RED;

        Vectors::Vec3 color_v_1 = {static_cast<float>(color1.r), static_cast<float>(color1.g), static_cast<float>(color1.b)};
        Vectors::Vec3 color_v_2 = {static_cast<float>(color2.r), static_cast<float>(color2.g), static_cast<float>(color2.b)};
        float t = prev_acceleration.length() / 100.f;
        t = std::fmin(1.f, t); //Clamp it between 0 and 1
        Vectors::Vec3 color_v = color_v_1.lerp(color_v_2, t);

        Color color = {static_cast<unsigned char>(color_v.x), static_cast<unsigned char>(color_v.y), static_cast<unsigned char>(color_v.z), 255};
        material.maps[MATERIAL_MAP_DIFFUSE].color = color;

        //DrawSphere(position, radius, color);
        //DrawCube(position, radius*2.f, radius*2.f, radius*2.f, color);
        DrawMesh(mesh, material, MatrixTranslate(position.x, position.y, position.z));

    }

}
