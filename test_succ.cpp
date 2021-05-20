// The only purpose of this file is to test code.

#include "successor.h"
#include <map>
#include <string>
#include <stdexcept>
#include <optional>
#include <iostream>
#define assert1(cond) if (!(cond)) {throw std::logic_error("Assertion failed: " #cond);}
#define assert2(cond, str) if (!(cond)) {throw std::logic_error(str);}

namespace test {
    std::map<std::string, void(*)()> tests {
        {"successor_new", [](){
            MinHeap<int, int, true> heap(nullptr);
            assert1(heap.min() == std::nullopt);
        }},

        {"getCertificate", [](){
            MinHeap<int, int, true> heap(nullptr);
            heap.add(8, 0);
            assert1(heap.min() == std::make_optional(8));
            assert1(heap.remove_min() == std::make_optional(8));
            assert1(heap.remove_min() == std::nullopt);
        }},

        {"heap_add_second", [](){
            MinHeap<int, int, true> heap(nullptr);
            heap.add(8, 0);
            heap.add(5, 0);
            assert1(heap.remove_min() == std::make_optional(5));
            assert1(heap.remove_min() == std::make_optional(8));
            assert1(heap.remove_min() == std::nullopt);
        }},

        {"heap_add_multiple", [](){
            MinHeap<int, int, true> heap(nullptr);
            for (int i : std::array<int, 10>{5, 10, 4, 3, 7, 2, 1, 8, 6, 9})
                heap.add(i, 0);
            for (int i = 1; i <= 10; ++i)
                assert2(heap.remove_min() == std::make_optional(i), "Failed to remove " + std::to_string(i));
            assert1(heap.remove_min() == std::nullopt);
        }},

        {"heap_add_after_remove", [](){
            MinHeap<int, int, true> heap(nullptr);
            for (int i : std::array<int, 5>{5, 10, 4, 3, 7})
                heap.add(i, 0);
            heap.remove_min();
            heap.add(2, 0);
            for (int i : std::array<int, 5>{2, 4, 5, 7, 10})
                assert2(heap.remove_min() == std::make_optional(i), "Failed to remove " + std::to_string(i));
            assert1(heap.remove_min() == std::nullopt);
        }},

        {"kinetic_heap_new", [](){
            KineticHeap<int> heap(std::vector<MovingObject<int>>{
                MovingObject(0, 1, 2),
            });
            assert1(heap.min().value().value == 2);
        }},

        {"kinetic_heap_one_takeover", [](){
            KineticHeap<int> heap(std::vector<MovingObject<int>>{
                MovingObject(0, 1, 2),
                MovingObject(10, -3, 3),
            });
            assert1(heap.min().value().value == 2);
            heap.fastforward(2);
            assert1(heap.min().value().value == 2);
            heap.fastforward(1);
            assert1(heap.min().value().value == 3);
        }},

        {"kinetic_heap_parallel", [](){
            KineticHeap<int> heap(std::vector<MovingObject<int>>{
                MovingObject(0, 1, 2),
                MovingObject(1, 1, 3),
                MovingObject(2, 1, 4),
                MovingObject(3, 1, 5),
            });
            assert1(heap.min().value().value == 2);
            heap.fastforward(1 << 20);
            assert1(heap.min().value().value == 2);
        }},

        {"kinetic_heap_total_certificate_invalidation", [](){
            KineticHeap<int> heap(std::vector<MovingObject<int>>{
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
            assert1(heap.min().value().value == 2);
            heap.fastforward(1); // Invalidates 5 certificates
            assert1(heap.min().value().value == 2);
            heap.fastforward(1 << 20);
            assert1(heap.min().value().value == 9);
        }},

        {"kinetic_heap_reverse", [](){
            std::vector<MovingObject<int>> vec;
            for (int i = 0; i <= 10; ++i)
                vec.push_back(MovingObject<int>(i, 10 - 2 * i, i));
            KineticHeap<int> heap(vec);

            assert1(heap.min().value().value == 0);
            heap.fastforward(1);
            assert1(heap.min().value().value == 10);
        }},

        {"kinetic_successor_two_items", [](){
            std::vector<MovingObject<int>> vec;
            assert1(true);
        }}
    };
}

int main(int argc, char** argv) {
    for (auto pair : test::tests) {
        //if (pair.first == "kinetic_heap_total_certificate_invalidation") {
            try {
                pair.second();
                std::cout << "Passed: " << pair.first << std::endl;
            } catch (std::logic_error e) {
                std::cout << "Failed: " << pair.first << std::endl;
                std::cout << "    " << e.what() << std::endl;
            }
        //}
    }
}
