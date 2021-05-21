// The purpose of this file is to test our kinetic successors against Brute-force implementation.

#include "successor.h"
#include <map>
#include <string>
#include <stdexcept>
#include <optional>
#include <iostream>
#include <time.h>
#include <experimental/random>
#include <utility>
#include <algorithm>
#define assert1(cond) if (!(cond)) {throw std::logic_error("Assertion failed: " #cond);}
#define assert2(cond, str) if (!(cond)) {throw std::logic_error(str);}

int main(int argc, char** argv) {
    const int num_particle = 50000;
    int time = 0;
    std::array<int, num_particle> velocities {};
    std::array<int, num_particle> positions {};
    std::array<int, num_particle> values {};
    // MinHeap<int, int, true> heap(nullptr);
    // for (int i : positions)
    //     heap.add(i, 0);
    // Randomly generate the initial positions and velocities (integers)
    for (int i = 0; i < positions.size(); i++){
        positions[i] = std::experimental::randint(-5000, 5000);
    }
    for (int i = 0; i < velocities.size(); i++){
        velocities[i] = std::experimental::randint(-1000, 1000);
    }

    for (int i = 0; i < values.size(); i++){
        values[i] = i;
    }

    std::vector<MovingObject<int>> moving_objs;
    for (int k=0; k < num_particle; k++){
        MovingObject<int> moving_obj = MovingObject(positions[k], velocities[k], &time, values[k]);
        moving_objs.push_back(moving_obj);
    }


    // Obtain the running times for each time increment
    const int qnum = 5;           // number of queries
    std::array<int, qnum> time_incs {10, 100, 500, 1000, 10000};
    std::vector<MovingObject<int>> query {};
    // Generate queries
    for (int i = 0; i < qnum; i++){
        int idx = std::experimental::randint(0, (int)moving_objs.size());
        query.push_back(moving_objs[idx]);
    }
    std::vector<MovingObject<int>> moving_obj_brute = moving_objs; // Record the particles for Brute-force method
    double time_sum = 0;
    double kinetic_time_sum = 0;
    // std::optional succ;
    for (int j = 0; j < time_incs.size(); j++){
        int time_inc = time_incs[j]; 
    // Update the position array for the Brute-force method
        clock_t start = clock();
        for (int k = 0; k < moving_obj_brute.size(); k++){
            *(moving_obj_brute[k].curtime) = *(moving_obj_brute[k].curtime) + time_inc;
        }
        sort(moving_obj_brute.begin(), moving_obj_brute.end());
        for (int k = 0; k < moving_obj_brute.size(); k++){
            if (moving_obj_brute[k].value == query[j].value)
            std::optional    succ = k < moving_obj_brute.size()-1 ? std::make_optional(moving_obj_brute[k+1].value) : std::nullopt; 
        }
        clock_t end= clock();
        // std::cout << j << " [Naive]" << "succ: "<< succ.value << " time: "<< (end - start) * 1.0 / CLOCKS_PER_SEC*1000 << "ms" << std::endl;
        std::cout << j << " [Naive]" << " time: "<< (end - start) * 1.0 / CLOCKS_PER_SEC*1000 << "ms" << std::endl;
        time_sum += (end - start) * 1.0 / CLOCKS_PER_SEC*1000;
        time_sum += (end - start) * 1.0 / CLOCKS_PER_SEC*1000;
        // Update the kinetic heap and the heap min with a running time
    
        KineticSuccessor kinetic_successor(moving_objs, &time);
        clock_t kinetic_start = clock();
        kinetic_successor.fastforward(time_inc);
        std::optional kinetic_succ = kinetic_successor.findSuccessor(query[j]);
        clock_t kinetic_end = clock();
        // std::cout << j << " [Kinetic]" << "succ: "<< kinetic_succ.value().value << " time: "<< (kinetic_end - kinetic_start) * 1.0 / CLOCKS_PER_SEC*1000 << "ms" << std::endl;
        std::cout << j << " [Kinetic]" << " time: "<< (kinetic_end - kinetic_start) * 1.0 / CLOCKS_PER_SEC*1000 << "ms" << std::endl;
        kinetic_time_sum += (kinetic_end - kinetic_start) * 1.0 / CLOCKS_PER_SEC*1000;
    }
    std::cout << "Naive heap total time:" << time_sum << "ms" << std::endl;
    std::cout << "Kinetic heap total time:" << kinetic_time_sum << "ms" << std::endl;

}
