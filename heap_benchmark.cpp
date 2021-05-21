// The purpose of this file is to test our kinetic heap against the naive heap with Brute-force implementation.

#include "heap.h"
#include <map>
#include <string>
#include <stdexcept>
#include <optional>
#include <iostream>
#define assert1(cond) if (!(cond)) {throw std::logic_error("Assertion failed: " #cond);}
#define assert2(cond, str) if (!(cond)) {throw std::logic_error(str);}

int main(int argc, char** argv) {
    std::array<int, 9> velocities {2, 4, 0, -4, 0, 0, 0, -9, -9};
    std::array<int, 9> positions {0, 2, 5, 6, 9, 10, 11, 16, 17};
    // MinHeap<int, int, true> heap(nullptr);
    // for (int i : positions)
    //     heap.add(i, 0);
    KineticHeap<int> kinetic_heap(std::vector<MovingObject<int>>{
                MovingObject(0, 2, 2),
                MovingObject(2, 4, 3),
                MovingObject(5, 0, 4),
                MovingObject(6, -4, 5),
                MovingObject(9, 0, 6),
                MovingObject(10, 0, 7),
                MovingObject(11, 0, 8),
                MovingObject(16, -9, 9),
                MovingObject(17, -9, 10),
            });


    // Obtain the min values and running times for each time increment
    std::array<int, 5> time_incs {10, 100, 500, 1000, 10000};
    double heap_time_sum = 0;
    double kinetic_heap_time_sum = 0;
    for (int j = 0; j < time_incs.size(); j++){
        int time_inc = time_incs[j]; 
    // Update the position array for the naive heap
        for (int k = 0; k < positions.size(); k++){
            auto pos = positions[k];
            positions.at(k) = pos + time_inc *velocities[k];
        }
        // Update the naive heap and the heap min with a running time
        clock_t heap_start = clock();
        MinHeap<int, int, true> heap(nullptr);
        for (int i : positions)
            heap.add(i, 0);
        std::optional heap_min = heap.min();
        clock_t heap_end= clock();
        //std::cout << j << " [Naive heap]" << "min: "<< heap_min.value().value << " time: "<< (heap_end - heap_start) * 1.0 / CLOCKS_PER_SEC*1000 << "ms" << std::endl;
        heap_time_sum += (heap_end - heap_start) * 1.0 / CLOCKS_PER_SEC*1000;
        // Update the kinetic heap and the heap min with a running time
        clock_t kinetic_heap_start = clock();
        kinetic_heap.fastforward(time_inc);
        int kinetic_heap_min = kinetic_heap.min().value().value;
        clock_t kinetic_heap_end = clock();
        //std::cout << j << " [Kinetic heap]" << "min: "<< kinetic_heap_min << " time: "<< (kinetic_heap_end - kinetic_heap_start) * 1.0 / CLOCKS_PER_SEC*1000 << "ms" << std::endl;
        kinetic_heap_time_sum += (kinetic_heap_end - kinetic_heap_start) * 1.0 / CLOCKS_PER_SEC*1000;
    }
    std::cout << "Naive heap total time:" << heap_time_sum << "ms" << std::endl;
    std::cout << "Kinetic heap total time:" << kinetic_heap_time_sum << "ms" << std::endl;


    // for (auto pair : test::tests) {
    //     //if (pair.first == "kinetic_heap_total_certificate_invalidation") {
    //         try {
    //             pair.second();
    //             std::cout << "Passed: " << pair.first << std::endl;
    //         } catch (std::logic_error e) {
    //             std::cout << "Failed: " << pair.first << std::endl;
    //             std::cout << "    " << e.what() << std::endl;
    //         }
    //     //}
    // }
}
