#include <vector>
#include <utility>
#include <optional>

template<typename T>
struct MinHeap {
    std::vector<T> vec;

    // Add element to simplify parent-child math
    MinHeap() : vec(1) {}

    static constexpr size_t root() {
        return 1;
    }

    static constexpr size_t sibling(size_t i) {
        return i ^ 1;
    }

    static constexpr size_t parent(size_t i) {
        return i / 2;
    }

    static constexpr size_t left(size_t i) {
        return 2 * i;
    }

    static constexpr size_t right(size_t i) {
        return 2 * i + 1;
    }

    // Gets the minimum element if it exists
    std::optional<T> min() {
        return vec.size() > root() ? std::make_optional(vec[root()]) : std::nullopt;
    }

    // Adds an element to the heap
    void add(T t) {
        size_t index = vec.size();
        vec.push_back(t);

        // Heap up
        while (index > root()) {
            size_t parent_index = parent(index);
            if (t < vec[parent_index]) {
                std::swap(vec[index], vec[parent_index]);
            } else {
                break;
            }
            index = parent_index;
        }
    }

    // Gets the minimum element and removes it if it exists
    std::optional<T> remove_min() {
        std::optional<T> min_elem = min();
        if (min_elem.has_value()) {
            vec[root()] = vec[vec.size() - 1];
            vec.pop_back();

            // Heap down
            size_t index = root();
            while (left(index) < vec.size()) {
                // Find smaller child
                bool has_right = right(index) < vec.size();
                size_t smaller = has_right && vec[right(index)] < vec[left(index)] ? right(index) : left(index);

                if (vec[smaller] < vec[index]) {
                    std::swap(vec[index], vec[smaller]);
                } else {
                    break;
                }
                index = smaller;
            }
        }

        return min_elem;
    }
};