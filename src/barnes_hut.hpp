#pragma once

#include <array>
#include <memory>

#include "particle.hpp"
#include "vectors.hpp"

namespace BarnesHut {

    constexpr std::size_t top_front_left_idx = 0;
    constexpr std::size_t top_front_right_idx = 1;
    constexpr std::size_t top_back_left_idx = 2;
    constexpr std::size_t top_back_right_idx = 3;
    constexpr std::size_t bottom_front_left_idx = 4;
    constexpr std::size_t bottom_front_right_idx = 5;
    constexpr std::size_t bottom_back_left_idx = 6;
    constexpr std::size_t bottom_back_right_idx = 7;

    //Assumed to be axis aligned
    class Box {

    public:
        float x_min, y_min, z_min;
        float x_max, y_max, z_max;

        bool is_point_inside(const Vectors::Vec3 &p) const;
        bool overlaps(const Box &box) const;

    };

    class Node {

        bool has_particle = false;  //Has the node recieved a particle thus far?
        bool has_sub_nodes = false; //Does the node have any allocated sub nodes?
        
        //The first particle to be inserted into the node. Important for when a second particle gets inserted, since then the first one must be re-inserted
        Particle::Particle first_particle;
        bool reinserted_first_particle = false; //Has the first particle been re-inserted yet?
        
        Vectors::Vec3 position = {0.f, 0.f, 0.f}; //Weighted by the mass of every particle within the node
        float mass = 0.f;

        Box bounding_box;

        std::array<Box, 8> create_sub_node_boxes() const;

    public:

        //A 3D barnes hut tree is an octtree
        std::array< std::unique_ptr<Node>, 8 > sub_nodes;

        inline Node() {
            for (std::size_t i = 0; i < sub_nodes.size(); i++) sub_nodes[i] = nullptr;
        }
        
        void insert_particle(const Particle::Particle &particle);
        void apply_gravity(Particle::Particle *const particles, std::size_t particle_idx, std::size_t n_particles, float G) const;

        void render() const;

        friend class Tree;

    };

    class Tree {

        std::unique_ptr<Node> root_node = nullptr;

    public:

        void insert_particle(const Particle::Particle &particle);
        void apply_gravity(Particle::Particle *const particles, std::size_t particle_idx, std::size_t n_particles, float G) const;

        void render() const;        

    };

}
