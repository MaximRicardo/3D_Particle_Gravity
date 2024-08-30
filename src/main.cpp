#include <array>
#include <cstdio>
#include <thread>
#include <vector>
#include <cmath>

#include "raylib.h"
#include "vectors.hpp"
#include "particle.hpp"
#include "barnes_hut.hpp"

constexpr float G = 1.f;

void simulate_particles(std::vector<Particle::Particle> &particles, std::size_t lower_limit, std::size_t upper_limit, const BarnesHut::BarnesHutTree &bh_tree) {

    for (std::size_t i = lower_limit; i <= upper_limit; i++) {

        bh_tree.apply_gravity(particles.data(), i, particles.size(), G);

        for (std::size_t j = 0; j < particles.size(); j++) {
            if (i == j) continue;
            particles[i].collision(particles[j]);
        }

    }

}

int main() {

    SetTraceLogLevel(TraceLogLevel::LOG_ERROR);

    constexpr unsigned windowed_screen_width = 1280;
    constexpr unsigned windowed_screen_height = 720;

    unsigned screen_width = windowed_screen_width;
    unsigned screen_height = windowed_screen_height;
    InitWindow(screen_width, screen_height, "Gravity Sim");

    Camera camera = {{0.f, 0.f, -100.f}, {0.f, 0.f, 100.f}, {0.f, 1.f, 0.f}, 59.f, CAMERA_PERSPECTIVE};
    float camera_move_speed = 20.f;

    std::vector<Particle::Particle> particles;
    {
        std::size_t n_particles = 3000;
        particles.reserve(n_particles);

        std::array<float, 2> sphere_particles_fraction = {4.f/5.f, 1.f/5.f};
        for (std::size_t n = 0; n < 2; n++) {

            Vectors::Vec3 center;
            if (n == 0) center = {0.f, 0.f, 0.f};
            else center = {-100.f, 0.f, 0.f};

            float theta = 0.f;  //Azimuthal angle
            float phi = 0.f;  //Altitude angle
            float dist = 5.f;
            float dist_inc = 5.f;
            for (std::size_t i = 0; static_cast<float>(i) < static_cast<float>(n_particles)*sphere_particles_fraction[n]; i++) {
                Particle::Particle particle;
                
                particle.position = {std::sin(phi)*std::cos(theta)*dist, std::sin(phi)*std::sin(theta)*dist, std::cos(phi)*dist}; //Spherical to cartesian coordinates
                particle.position = particle.position + center;
                theta += 0.5f;
                if (theta >= 2.f*PI) {
                    phi += 0.5f;
                    theta -= 2.f*PI;
                }
                if (phi >= 2.f*PI) {
                    dist += dist_inc;
                    phi -= 2.f*PI;
                }

                particle.velocity = {0.f, 0.f, 0.f};
                particle.acceleration = {0.f, 0.f, 0.f};
                particle.prev_acceleration = {0.f, 0.f, 0.f};
                particle.mass = 10.f;
                particle.radius = 1.f;

                particles.push_back(particle);
            }
        }
    }

    //Mesh mesh = GenMeshCube(2.f, 2.f, 2.f);
    Mesh mesh = GenMeshSphere(1.f, 10, 10);

    Material mat_default = LoadMaterialDefault();

    std::size_t n_simulation_threads = std::thread::hardware_concurrency() / 2;
    if (n_simulation_threads == 0) {
        std::fprintf(stderr, "WARNING: Unable to detect maximum number of concurrent threads supported! Using 1 thread.\n");
        n_simulation_threads = 1;
    }
    
    std::printf("Simulation using %zu threads.\n", n_simulation_threads);

    DisableCursor();

    while (!WindowShouldClose()) {

        float delta_time = GetFrameTime();

        UpdateCameraPro(&camera, 
                {
                    IsKeyDown(KEY_W)*delta_time*camera_move_speed -
                    IsKeyDown(KEY_S)*delta_time*camera_move_speed,
                    IsKeyDown(KEY_D)*delta_time*camera_move_speed -
                    IsKeyDown(KEY_A)*delta_time*camera_move_speed,
                    IsKeyDown(KEY_E)*delta_time*camera_move_speed -
                    IsKeyDown(KEY_Q)*delta_time*camera_move_speed
                },
                {
                    GetMouseDelta().x*0.05f,
                    GetMouseDelta().y*0.05f,
                    0.0f
                },
                0.f);

        if (IsKeyPressed(KEY_ENTER) && IsWindowFullscreen()) {
            screen_width = windowed_screen_width;
            screen_height = windowed_screen_height;
            SetWindowSize(screen_width, screen_height);
            ToggleFullscreen();
        }
        else if (IsKeyPressed(KEY_ENTER) && !IsWindowFullscreen()) {
            int current_screen = GetCurrentMonitor();
            screen_width = GetMonitorWidth(current_screen);
            screen_height = GetMonitorHeight(current_screen);
            ToggleFullscreen();
            SetWindowSize(screen_width, screen_height);
        }

        if (IsKeyPressed(KEY_R)) {
            Particle::Particle new_particle;

            new_particle.position = {camera.position.x, camera.position.y, camera.position.z};

            new_particle.velocity = {25.f, 0.f, 0.f};
            new_particle.acceleration = {0.f, 0.f, 0.f};
            new_particle.prev_acceleration = {0.f, 0.f, 0.f};
            new_particle.radius = 1.f;
            new_particle.mass = 10.f;

            particles.push_back(new_particle);
        }

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);

        BarnesHut::BarnesHutTree bh_tree;

        //Construct the bh tree
        for (std::size_t i = 0; i < particles.size(); i++) {
            bh_tree.insert_particle(particles[i]);
        }

        if (IsKeyDown(KEY_SPACE)) bh_tree.render();

        {
            std::size_t n_threads = n_simulation_threads;
            std::vector<std::thread> threads;
            threads.reserve(n_threads);

            std::size_t limit_inc_amount = static_cast<std::size_t>(std::ceil(static_cast<float>(particles.size()) / static_cast<float>(n_threads)));
            for (std::size_t i = 0; i < n_threads; i++) {
                std::size_t lower_limit = limit_inc_amount * i;
                std::size_t upper_limit = limit_inc_amount * (i+1) - 1;
                upper_limit = std::fmin(upper_limit, particles.size()-1);
                threads.emplace_back(simulate_particles, std::ref(particles), lower_limit, upper_limit, std::ref(bh_tree));
            }

            for (std::size_t i = 0; i < n_threads; i++) threads[i].join();
        }

        std::size_t n_particles_near_origin = 0;
        for (std::size_t i = 0; i < particles.size(); i++) {

            particles[i].update(delta_time);

            particles[i].draw(mesh, mat_default);

            if (particles[i].position.dist({0.f, 0.f, 0.f}) < 500.f) ++n_particles_near_origin;
        }

        if (IsKeyDown(KEY_Z)) DrawSphere({0.f, 0.f, 0.f}, 500.f, {GOLD.r, GOLD.g, GOLD.b, 127});

        EndMode3D();

        DrawFPS(10, 10);
        DrawText(TextFormat("%zu/%zu Particles within 500 units of the origin.", n_particles_near_origin, particles.size()), 10, 50, 20, WHITE);
        EndDrawing();

    }

    EnableCursor();

    CloseWindow();

    return 0;

}
