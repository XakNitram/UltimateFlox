module;
#include "pch.hpp"
export module ThreadedAlgorithm;

export import Algorithm;
import Boid;
import Boidtree;
import DoubleBuffer;
import RawArray;
import Rectangle;

constexpr ptrdiff_t BOID_GROUP = 8;


export class ThreadedAlgorithm;


struct ThreadWork {
    ThreadedAlgorithm *algorithm;
    int id;
    float delta;
    const Boid *read;
    Boid *write;
    ptrdiff_t count;
    ptrdiff_t start;

    ThreadWork(ThreadedAlgorithm *a, int i, float d, const Boid *r, Boid *w, ptrdiff_t c, ptrdiff_t s) :
        algorithm(a), id(i), delta(d), read(r), write(w), count(c), start(s) {}

    void operator()() const;
};


export class ThreadedAlgorithm final : public Algorithm {
    void populate_tree(const Boid *read, const ptrdiff_t count) {
        m_tree.clear();
        m_tree.bounds = m_treeBounds;
        for (Boid const &boid: RawArray(read, count)) {
            m_tree.insert(&boid, boid.position);
        }
    }

    void distribute_work(const Boid *read, Boid *write, const ptrdiff_t count, const float delta) {
        // Maybe compute these values only when flock size changes?
        // Compiler should see div/mod and combine operations.
        // Number of groups of boids.
        const ptrdiff_t boid_groups = count / BOID_GROUP;

        // Number of boids outside a group. Always less than boid group size. Fine. I'll process them myself.
        const ptrdiff_t leftover_boids = count % BOID_GROUP;

        // Number of boid groups per thread. 0 if less than ThreadCount groups.
        const ptrdiff_t boid_groups_per_thread = boid_groups / ThreadCount;

        // Number of groups not evenly distributed across the threads. Evenly distribute them across the threads.
        ptrdiff_t leftover_groups = boid_groups % ThreadCount;
        const ptrdiff_t boids_per_thread = BOID_GROUP * boid_groups_per_thread;

        //std::cout << '\n'
        //          << boid_groups << " groups.\n"
        //          << boid_groups_per_thread << " groups per thread.\n"
        //          << leftover_groups << " leftover groups.\n"
        //          << leftover_boids << " leftover boids.\n";

        // Leftover boids have to be added to the last thread.
        // The last thread only takes a leftover group if there are no leftover boids.
        // If the first thread took two birds extra, each of the other threads' processing would start offset by two.

        const bool primary_thread_needs_work = !(boids_per_thread || leftover_boids);

        // See if we need to take a group here.
        const ptrdiff_t final_start {
            primary_thread_needs_work ?
            BOID_GROUP * (boid_groups_per_thread * (ThreadCount - 1) + leftover_groups - (leftover_groups > 0)) :
            BOID_GROUP * (boid_groups_per_thread * (ThreadCount - 1) + leftover_groups)
        };

        // Take the group here.
        const ptrdiff_t final_count {
            primary_thread_needs_work ?
            boids_per_thread + leftover_boids + (leftover_groups-- > 0) * BOID_GROUP :
            boids_per_thread + leftover_boids
        };

        ThreadFutures futures {};
        if (boid_groups_per_thread > 0 || leftover_groups > 0) {
            ptrdiff_t next_start = 0;
            for (int i = 0; i < ThreadCount - 1; ++i) {
                if (boid_groups_per_thread == 0 && leftover_groups <= 0) { break; }
                futures[i] = m_pool.submit(
                    [](ThreadWork thread_work) { thread_work(); },
                    ThreadWork {
                        this, i, delta,
                        read, write,
                        boids_per_thread + (leftover_groups > 0) * BOID_GROUP,  // I might take a group below.
                        next_start
                    }
                );

                next_start += boids_per_thread + (leftover_groups-- > 0) * BOID_GROUP;  // Take a group here.
            }
        }

        //Do my work.
        ThreadWork {this, ThreadCount - 1, delta, read, write, final_count, final_start}();

        // Wait for the others to finish their work.
        for (auto &update_future: futures) {
            if (update_future.valid()) {
                update_future.get();
            }
        }
    }

    void recalculate_bounds(Boid *write, const ptrdiff_t count) {
        // Separate loop for thread-safety. Does this slow the program down?
        // Maybe have an array where the results of this test from each thread are stored and join them after.
        Vector x_bound {m_treeBounds.center.x - m_treeBounds.size.x, m_treeBounds.center.x + m_treeBounds.size.x};
        Vector y_bound {m_treeBounds.center.y - m_treeBounds.size.y, m_treeBounds.center.y + m_treeBounds.size.y};
        for (auto const &current: RawArray(write, count)) {
            if (current.position.x < x_bound.r) {
                x_bound.r = current.position.x;
            } else if (current.position.x > x_bound.g) {
                x_bound.g = current.position.x;
            }

            if (current.position.y < y_bound.r) {
                y_bound.r = current.position.y;
            } else if (current.position.y > y_bound.g) {
                y_bound.g = current.position.y;
            }
        }

        float x_bound_center = (x_bound.r + x_bound.g) * 0.5f;
        float y_bound_center = (y_bound.r + y_bound.g) * 0.5f;

        Rectangle newBounds {
            {x_bound_center,             y_bound_center},
            {x_bound.g - x_bound_center, y_bound.g - y_bound_center}
        };

        m_treeBounds = newBounds;
    }

    friend ThreadWork;

public:
    explicit ThreadedAlgorithm(Vector b) : m_bounds(b), m_treeBounds(m_bounds), m_tree(m_treeBounds) {
        for (auto &m_result: m_results) {
            m_result.reserve(128);
        }

        m_pool.init();
    }

    ~ThreadedAlgorithm() override {
        m_pool.shutdown();
    }

    void update(DoubleBuffer<Boid> &boids, const float delta) override {
        const auto count = static_cast<ptrdiff_t>(boids.count());
        if (count == 0) { return; }

        const Boid *read = boids.read();
        Boid *write = boids.write();

        // Insert the boids into the quadtree
        populate_tree(boids.read(), count);

        // Distribute the calculation work evenly among the available threads.
        distribute_work(read, write, count, delta);

        // Recalculate the bounds of the quadtree to keep the birds inside.
        recalculate_bounds(boids.write(), count);
    }

    [[nodiscard]] Boidtree const &tree() const {
        return m_tree;
    }
private:
    static constexpr int ThreadCount = 8;
    using ThreadFutures = std::array<std::future<void>, ThreadCount - 1>;
    using QuadtreeResults = std::vector<Boid>;

    Rectangle m_bounds;
    Rectangle m_treeBounds;
    Boidtree m_tree;
    //std::mutex m_mutex;

    ThreadPool m_pool {ThreadCount - 1};
    QuadtreeResults m_results[ThreadCount];
};


void ThreadWork::operator()() const {
    //{
    //    std::unique_lock<std::mutex> lock(algorithm->m_mutex);
    //    std::cout << "Thread " << id << " processing " << count << " boids starting at " << start << ".\n";
    //}
    const Boidtree &tree = algorithm->m_tree;
    const Rectangle bounds = algorithm->m_bounds;
    auto &results = algorithm->m_results[id];
    const float disruptive_radius = Boid::disruptiveRadius * Boid::disruptiveRadius;
    const float cohesive_radius = Boid::cohesiveRadius * Boid::cohesiveRadius;

    const Rectangle center_bound{bounds * 0.75f};
    const Rectangle hard_bound{bounds * 0.90f};

    Rectangle search_bound{Vector{Boid::cohesiveRadius}};
    for (ptrdiff_t i = start; i < start + count; ++i) {
        const Boid current = read[i];
        const Boid *previous = read + i;

        search_bound.center = current.position;
        Vector center_steer{0.0f, 0.0f};

        const bool in_center = center_bound.contains(current.position);
        float center_steer_weight = Boid::primadonnaWeight;
        if (!in_center) {
            if (!hard_bound.contains(current.position)) {
                center_steer_weight *= 2.0f;
            }

            center_steer -= current.position;
            center_steer = steer(center_steer, current.velocity);
        }

        const Vector full_speed = steer(current.velocity, current.velocity);

        results.clear();
        search(tree, previous, search_bound, results);
        //std::sort(
        //    results.begin(), results.end(),
        //    [&current](const Boid &a, const Boid &b) {
        //        return glm::distance2(current.position, a.position) < glm::distance2(current.position, b.position);
        //    }
        //);

        Vector separation{0.0f, 0.0f};
        Vector alignment{0.0f, 0.0f};
        Vector cohesion{0.0f, 0.0f};
        size_t cohesive_total = 0;
        size_t disruptive_total = 0;

        for (const Boid &other : results) {
            const float d2 = glm::distance2(current.position, other.position);

            const size_t is_disruptive = d2 < disruptive_radius;
            const size_t is_cohesive = d2 < cohesive_radius;

            separation += FloatEnable[is_disruptive] * ((current.position - other.position) / (d2 + Epsilon));
            alignment += FloatEnable[is_cohesive] * other.velocity;
            cohesion += FloatEnable[is_cohesive] * other.position;

            disruptive_total += is_disruptive;
            cohesive_total += is_cohesive;

            //if (d2 < disruptiveRadius) {
            //    separation += (current.position - other.position) / d2 + epsilon;
            //    disruptive_total++;
            //}

            //if (d2 < cohesiveRadius) {
            //    alignment += other.velocity;
            //    cohesion += other.position;
            //    cohesive_total++;
            //}
        }

        if (disruptive_total > 0) {
            separation /= static_cast<float>(disruptive_total);
            separation = steer(separation, current.velocity);
        }

        if (cohesive_total > 0) {
            const float countFactor = 1.0f / static_cast<float>(cohesive_total);
            alignment *= countFactor;

            cohesion *= countFactor;
            cohesion -= current.position;

            alignment = steer(alignment, current.velocity);
            cohesion = steer(cohesion, current.velocity);
        }

        const Vector acceleration = magnitude(
            Vector{center_steer * center_steer_weight + full_speed * Boid::speedWeight +
                   separation * Boid::separationWeight + alignment * Boid::alignmentWeight +
                   cohesion * Boid::cohesionWeight},
            Boid::maxForce);

        write[i].velocity += acceleration;
        write[i].position += current.velocity * delta;
    }
}
