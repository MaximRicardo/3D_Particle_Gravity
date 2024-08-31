

#include "vectors.hpp"
#include "barnes_hut.hpp"

namespace {

    float lerp(float x, float y, float t) {
        return x + (y-x)*t;
    }

    float midpoint(float x, float y) {
        return lerp(x, y, 0.5f);
    }

}

namespace BarnesHut {

    bool Box::is_point_inside(const Vectors::Vec3 &p) const {

        return x_min <= p.x && p.x <= x_max &&
                y_min <= p.y && p.y <= y_max &&
                z_min <= p.z && p.z <= z_max;

    }
    
    bool Box::is_particle_maybe_inside(const Particle::Particle &particle) const {

        Vectors::Vec3 box_center;
        box_center.x = midpoint(x_min, x_max);
        box_center.y = midpoint(y_min, y_max);
        box_center.z = midpoint(z_min, z_max);

        float box_width = x_max-x_min;
        return particle.position.dist(box_center) <= box_width+particle.radius;
        
    }
    
    bool Box::overlaps(const Box &box) const {

        return x_min <= box.x_max && x_max >= box.x_min &&
            y_min < box.y_max && y_max > box.y_min &&
            z_min < box.z_max && z_max > box.z_min;

    }

    std::array<Box, 8> Node::create_sub_node_boxes() const {

        std::array<Box, 8> boxes;



        boxes[top_front_left_idx].x_min = bounding_box.x_min;
        boxes[top_front_left_idx].x_max = midpoint(bounding_box.x_min, bounding_box.x_max);

        boxes[top_front_left_idx].y_min = midpoint(bounding_box.y_min, bounding_box.y_max);
        boxes[top_front_left_idx].y_max = bounding_box.y_max;

        boxes[top_front_left_idx].z_min = midpoint(bounding_box.z_min, bounding_box.z_max);
        boxes[top_front_left_idx].z_max = bounding_box.z_max;
        //Done with the first box!! Now to do the other 7



        boxes[top_front_right_idx].x_min = midpoint(bounding_box.x_min, bounding_box.x_max);
        boxes[top_front_right_idx].x_max = bounding_box.x_max;
        
        boxes[top_front_right_idx].y_min = midpoint(bounding_box.y_min, bounding_box.y_max);
        boxes[top_front_right_idx].y_max = bounding_box.y_max;

        boxes[top_front_right_idx].z_min = midpoint(bounding_box.z_min, bounding_box.z_max);
        boxes[top_front_right_idx].z_max = bounding_box.z_max;



        boxes[top_back_left_idx].x_min = bounding_box.x_min;
        boxes[top_back_left_idx].x_max = midpoint(bounding_box.x_min, bounding_box.x_max);

        boxes[top_back_left_idx].y_min = midpoint(bounding_box.y_min, bounding_box.y_max);
        boxes[top_back_left_idx].y_max = bounding_box.y_max;

        boxes[top_back_left_idx].z_min = bounding_box.z_min;
        boxes[top_back_left_idx].z_max = midpoint(bounding_box.z_min, bounding_box.z_max);



        boxes[top_back_right_idx].x_min = midpoint(bounding_box.x_min, bounding_box.x_max);
        boxes[top_back_right_idx].x_max = bounding_box.x_max;
        
        boxes[top_back_right_idx].y_min = midpoint(bounding_box.y_min, bounding_box.y_max);
        boxes[top_back_right_idx].y_max = bounding_box.y_max;

        boxes[top_back_right_idx].z_min = bounding_box.z_min;
        boxes[top_back_right_idx].z_max = midpoint(bounding_box.z_min, bounding_box.z_max);



        //Bottom half
        
        boxes[bottom_front_left_idx].x_min = bounding_box.x_min;
        boxes[bottom_front_left_idx].x_max = midpoint(bounding_box.x_min, bounding_box.x_max);

        boxes[bottom_front_left_idx].y_min = bounding_box.y_min;
        boxes[bottom_front_left_idx].y_max = midpoint(bounding_box.y_min, bounding_box.y_max);

        boxes[bottom_front_left_idx].z_min = midpoint(bounding_box.z_min, bounding_box.z_max);
        boxes[bottom_front_left_idx].z_max = bounding_box.z_max;



        boxes[bottom_front_right_idx].x_min = midpoint(bounding_box.x_min, bounding_box.x_max);
        boxes[bottom_front_right_idx].x_max = bounding_box.x_max;
        
        boxes[bottom_front_right_idx].y_min = bounding_box.y_min;
        boxes[bottom_front_right_idx].y_max = midpoint(bounding_box.y_min, bounding_box.y_max);

        boxes[bottom_front_right_idx].z_min = midpoint(bounding_box.z_min, bounding_box.z_max);
        boxes[bottom_front_right_idx].z_max = bounding_box.z_max;



        boxes[bottom_back_left_idx].x_min = bounding_box.x_min;
        boxes[bottom_back_left_idx].x_max = midpoint(bounding_box.x_min, bounding_box.x_max);

        boxes[bottom_back_left_idx].y_min = bounding_box.y_min;
        boxes[bottom_back_left_idx].y_max = midpoint(bounding_box.y_min, bounding_box.y_max);

        boxes[bottom_back_left_idx].z_min = bounding_box.z_min;
        boxes[bottom_back_left_idx].z_max = midpoint(bounding_box.z_min, bounding_box.z_max);



        boxes[bottom_back_right_idx].x_min = midpoint(bounding_box.x_min, bounding_box.x_max);
        boxes[bottom_back_right_idx].x_max = bounding_box.x_max;
        
        boxes[bottom_back_right_idx].y_min = bounding_box.y_min;
        boxes[bottom_back_right_idx].y_max = midpoint(bounding_box.y_min, bounding_box.y_max);

        boxes[bottom_back_right_idx].z_min = bounding_box.z_min;
        boxes[bottom_back_right_idx].z_max = midpoint(bounding_box.z_min, bounding_box.z_max);



        return boxes;

    }

}
