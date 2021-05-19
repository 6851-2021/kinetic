// The only purpose of this file is to test code.

#include "heap.h"
#include <map>
#include <string>
#include <stdexcept>
#include <optional>
#include <iostream>
#define assert1(cond) if (!(cond)) {throw std::logic_error("Assertion failed: " #cond);}
#define assert2(cond, str) if (!(cond)) {throw std::logic_error(str);}

namespace test {
    std::map<std::string, void(*)()> tests {
        {"heap_new", [](){
            MinHeap<int, true> heap(nullptr);
            assert1(heap.min() == std::nullopt);
        }},

        {"heap_root", [](){
            MinHeap<int, true> heap(nullptr);
            heap.add(8, 0);
            assert1(heap.min() == std::make_optional(8));
            assert1(heap.remove_min() == std::make_optional(8));
            assert1(heap.remove_min() == std::nullopt);
        }},

        {"heap_add_second", [](){
            MinHeap<int, true> heap(nullptr);
            heap.add(8, 0);
            heap.add(5, 0);
            assert1(heap.remove_min() == std::make_optional(5));
            assert1(heap.remove_min() == std::make_optional(8));
            assert1(heap.remove_min() == std::nullopt);
        }},

        {"heap_add_multiple", [](){
            MinHeap<int, true> heap(nullptr);
            for (int i : std::array<int, 10>{5, 10, 4, 3, 7, 2, 1, 8, 6, 9})
                heap.add(i, 0);
            for (int i = 1; i <= 10; ++i)
                assert2(heap.remove_min() == std::make_optional(i), "Failed to remove " + std::to_string(i));
            assert1(heap.remove_min() == std::nullopt);
        }},

        {"heap_add_after_remove", [](){
            MinHeap<int, true> heap(nullptr);
            for (int i : std::array<int, 5>{5, 10, 4, 3, 7})
                heap.add(i, 0);
            heap.remove_min();
            heap.add(2, 0);
            for (int i : std::array<int, 5>{2, 4, 5, 7, 10})
                assert2(heap.remove_min() == std::make_optional(i), "Failed to remove " + std::to_string(i));
            assert1(heap.remove_min() == std::nullopt);
        }}
    };
}

int main(int argc, char** argv) {
    for (auto pair : test::tests) {
        try {
            pair.second();
            std::cout << "Passed: " << pair.first << std::endl;
        } catch (std::logic_error e) {
            std::cout << "Failed: " << pair.first << std::endl;
            std::cout << "    " << e.what() << std::endl;
        }
    }
}