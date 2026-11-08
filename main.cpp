#include <iterator>
#include <limits>
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <algorithm>
#include <random>

using Points   = std::vector<Vector2>;
using Clusters = std::vector<Points>;

const int WIDTH          = 800;
const int HEIGHT         = 800;
const int MAX_ITERATIONS = 1000;

Color GetClusterColor(int cluster_index) {
    switch (cluster_index) {
        case 0:  return RAYWHITE;
        case 1:  return BLUE;
        case 2:  return RED;
        case 3:  return GOLD;
        case 4:  return GREEN;
        case 5:  return YELLOW;
        default: return MAGENTA;
    }
}

Vector2 Vector2Mean(const std::vector<Vector2>& ps) {
    float sum_x = 0.0;
    float sum_y = 0.0;

    for (int i = 0; i < ps.size(); i++) {
        sum_x += ps[i].x;
        sum_y += ps[i].y;
    }

    Vector2 mean_p = {sum_x / ps.size(), sum_y / ps.size()};

    return mean_p;
}

int ClosestCentroid(Vector2 point, const Points& centroids) {
    float min_dist = std::numeric_limits<float>::max();
    int closest = 0;

    for (int j = 0; j < centroids.size(); j++) {
        float dist = Vector2DistanceSqr(point, centroids[j]);
        if (dist < min_dist) {
            min_dist = dist;
            closest = j;
        }
    }

    return closest;
}

void k_means(const Points& ps, Clusters& cs, int cluster_count) {
    if (ps.empty()) {
        cs.clear();
        return;
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());

    int k = std::min(int(ps.size()), cluster_count);

    Points centroids;
    std::sample(ps.begin(), ps.end(), std::back_inserter(centroids), k, gen);

    cs.resize(k);

    for (int i = 0; i < MAX_ITERATIONS; i++) {
        cs.assign(k, {});

        for (const auto& point : ps) {
            int closest = ClosestCentroid(point, centroids);
            cs[closest].push_back(point);
        }

        bool converged = true;

        for (int j = 0; j < k; j++) {
            Vector2 new_centroid;
            if (cs[j].empty()) {
                std::sample(ps.begin(), ps.end(), &new_centroid, 1, gen);
            } else {
                new_centroid = Vector2Mean(cs[j]);
            }

            if (Vector2DistanceSqr(centroids[j], new_centroid) > 1e-4) {
                converged = false;
            }

            centroids[j] = new_centroid;
        }

        if (converged) break;
    }

    cs.assign(k, {});

    for (const auto& point : ps) {
        int closest = ClosestCentroid(point, centroids);
        cs[closest].push_back(point);
    }
}

int main() {
    InitWindow(WIDTH, HEIGHT, "K-Means Visualization");

    Points points;
    Clusters clusters;
    int cluster_count = 1;


    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            points.push_back(GetMousePosition());
            k_means(points, clusters, cluster_count);
        }

        if (IsKeyPressed(KEY_SPACE)) {
            points.clear();
            clusters.clear();
            cluster_count = 1;
        }

        if (IsKeyPressed(KEY_UP)) {
            if (cluster_count < points.size()) {
                cluster_count++;
                k_means(points, clusters, cluster_count);
            }
        }

        if (IsKeyPressed(KEY_DOWN)) {
            if (cluster_count > 1) {
                cluster_count--;
                k_means(points, clusters, cluster_count);
            }
        }

        BeginDrawing();
        ClearBackground(GetColor(0x181818FF));

        DrawText(TextFormat("Clusters: %d", cluster_count), 30, 30, 30, RAYWHITE);
        DrawText(TextFormat("Points:   %d", points.size()), 30, 60, 30, RAYWHITE);

        if (!points.empty()) {
            for (int i = 0; i < clusters.size(); i++) {
                for (const auto& point : clusters[i]) {
                    DrawCircleV(point, 5, GetClusterColor(i));
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
