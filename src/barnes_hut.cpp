#include <array>
#include <cassert>
#include <limits>
#include <memory>
#include <cstdio>
#include <raylib.h>

#include "barnes_hut.hpp"
#include "particle.hpp"

namespace BarnesHut {

    void Tree::insert_particle(Particle::Particle &particle) {

        if (root_node == nullptr) {
            root_node = std::make_unique<Node>();
            
            root_node->bounding_box.x_min = -5000.f;
            root_node->bounding_box.x_max = 5000.f;
            root_node->bounding_box.y_min = -5000.f;
            root_node->bounding_box.y_max = 5000.f;
            root_node->bounding_box.z_min = -5000.f;
            root_node->bounding_box.z_max = 5000.f;
        }

        if (!root_node->bounding_box.is_point_inside(particle.position)) return;

        root_node->insert_particle(particle);

    }
    
    void Tree::apply_gravity(Particle::Particle *const particles, std::size_t particle_idx, std::size_t n_particles, float G) const {

        if (root_node == nullptr) return;

        root_node->apply_gravity(particles, particle_idx, n_particles, G);

    }
    
    void Tree::render() const {

        if (root_node == nullptr) return;

        root_node->render();

    }
    
    std::vector<Particle::Particle*> Tree::query(const Box &range) const {

        if (root_node == nullptr) return {};

        std::vector<Particle::Particle*> found;
        root_node->query(found, range);

        return found;

    }

    void Node::insert_particle(Particle::Particle &particle) {

        //Add the particle to the total mass and center of mass
        mass += particle.mass;
        position = position + particle.position*particle.mass;

        if (!has_particle) {
            has_particle = true;
            first_particle = &particle;
            return;
        }
        else if (particle.position.dist(first_particle->position) < 0.001f) return;

        //Else, create every sub node's bounding box to check the particle against
        std::array<Box, 8> sub_boxes = create_sub_node_boxes();

        //Check which box the particle is within
        std::size_t point_box_idx = std::numeric_limits<std::size_t>::max();
        for (std::size_t i = 0; i < sub_boxes.size(); i++) {
            if (sub_boxes[i].is_point_inside(particle.position)) {
                point_box_idx = i;
                break;
            }
        }
        assert(point_box_idx != std::numeric_limits<std::size_t>::max());

        //Continue from the node the particle is within
        
        if (sub_nodes[point_box_idx] == nullptr) {
            has_sub_nodes = true;
            sub_nodes[point_box_idx] = std::make_unique<Node>();
            sub_nodes[point_box_idx]->bounding_box = sub_boxes[point_box_idx];
        }

        sub_nodes[point_box_idx]->insert_particle(particle);


        if (reinserted_first_particle) return;

        //Do the same with the first particle, to re insert it into one of the sub nodes
        point_box_idx = std::numeric_limits<std::size_t>::max();
        for (std::size_t i = 0; i < sub_boxes.size(); i++) {
            if (sub_boxes[i].is_point_inside(first_particle->position)) {
                point_box_idx = i;
                break;
            }
        }
        assert(point_box_idx != std::numeric_limits<std::size_t>::max());

        if (sub_nodes[point_box_idx] == nullptr) {
            sub_nodes[point_box_idx] = std::make_unique<Node>();
            sub_nodes[point_box_idx]->bounding_box = sub_boxes[point_box_idx];
        }

        sub_nodes[point_box_idx]->insert_particle(*first_particle);
        
        reinserted_first_particle = true;

    }

    void Node::apply_gravity(Particle::Particle *const particles, std::size_t particle_idx, std::size_t n_particles, float G) const {

        Vectors::Vec3 center_of_mass = position/mass;
        float bounding_box_width = bounding_box.x_max - bounding_box.x_min; //Assumes the box to be equally wide in every axis

        float ratio = bounding_box_width / particles[particle_idx].position.dist(center_of_mass);
        if (ratio <= 1.f || !has_sub_nodes) {
            //If the ratio is <= 1, the node is sufficiently far away, and will be simplified to only be a point with pos = center_of_mass, and mass = mass
            //Also, if the node has no sub nodes, then there is no option but to use it's COM and mass
            
            if (center_of_mass.dist(particles[particle_idx].position) < 0.1f) return;

            Particle::Particle node_particle;
            node_particle.mass = mass;
            node_particle.position = center_of_mass;
            particles[particle_idx].apply_gravity(node_particle, G);
        }
        else {
            //Apply gravity using all of the existing sub nodes
            for (std::size_t i = 0; i < sub_nodes.size(); i++) {
                if (sub_nodes[i] != nullptr) sub_nodes[i]->apply_gravity(particles, particle_idx, n_particles, G);
            }
        }

    }
    
    void Node::render() const {

        for (std::size_t i = 0; i < sub_nodes.size(); i++) {
            if (sub_nodes[i] == nullptr) continue;
            sub_nodes[i]->render();
        }

        Vectors::Vec3 bounding_box_min = {bounding_box.x_min, bounding_box.y_min, bounding_box.z_min};
        Vectors::Vec3 bounding_box_max = {bounding_box.x_max, bounding_box.y_max, bounding_box.z_max};

        Vectors::Vec3 node_center = bounding_box_min.lerp(bounding_box_max, 0.5f);
        DrawCubeWires(node_center, bounding_box.x_max-bounding_box.x_min, bounding_box.y_max-bounding_box.y_min, bounding_box.z_max-bounding_box.z_min, WHITE);

    }
    
    void Node::query(std::vector<Particle::Particle*> &found, const Box &range) const {

        if (!bounding_box.overlaps(range)) return;

        if (!reinserted_first_particle && range.is_particle_maybe_inside(*first_particle)) found.push_back(first_particle);

        if (found.size() % 2 == 0) {
            for (std::size_t i = 0; i < sub_nodes.size(); i++) {
                if (sub_nodes[i] != nullptr) sub_nodes[i]->query(found, range);
            }
        }
        else {
            for (std::size_t i = sub_nodes.size()-1; i >= 0; i--) {
                if (sub_nodes[i] != nullptr) sub_nodes[i]->query(found, range);
                if (i == 0) break;
            }
        }

    }

}
