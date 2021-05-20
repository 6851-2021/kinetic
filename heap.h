#include <vector>
#include <utility>
#include <optional>
#include <ostream>
#include <iostream>
#include "successor.h"

template<typename T, typename Ref, bool Standalone = false>
struct MinHeap {
    struct Element {
        T t;
        // Index to the element to reference in the ref.
        // If this is 0, then no element gets referenced.
        // Also, not using pointers because vector pointers get invalidated on reallocation
        size_t ref_index;
    };

    std::vector<Element> vec;
    // The structure to reference.
    MinHeap<Ref, T, Standalone>* ref;

    // Add element to simplify parent-child math
    MinHeap(MinHeap<Ref, T, Standalone>* ref_) : vec(1), ref(ref_) {}

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
    std::optional<T> min() const {
        return vec.size() > root() ? std::make_optional(vec[root()].t) : std::nullopt;
    }

    std::optional<size_t> min_ref_index() const {
        return vec.size() > root() ? std::make_optional(vec[root()].ref_index) : std::nullopt;
    }

    // Swaps two elements and updates ref indexes
    void swap(size_t i, size_t j) {
        std::swap(vec[i], vec[j]);
        if constexpr (!Standalone) {
            ref->set_ref_index(vec[i].ref_index, i);
            ref->set_ref_index(vec[j].ref_index, j);
        }
    }

    // Gets the ref_index of element i, assuming i != 0
    size_t ref_index(size_t i) const {
        return vec[i].ref_index;
    }

    void set_ref_index(size_t i, size_t ref_index) {
        if (i != 0)
            vec[i].ref_index = ref_index;
    }

    // Heap up the element at some index and return its new index
    size_t heap_up(size_t index) {
        while (index > root()) {
            size_t parent_index = parent(index);
            if (vec[index].t < vec[parent_index].t) {
                swap(index, parent_index);
            } else {
                break;
            }
            index = parent_index;
        }
        return index;
    }

    // Heap down the element at some index and return its new index
    size_t heap_down(size_t index) {
        while (left(index) < vec.size()) {
            // Find smaller child
            bool has_right = right(index) < vec.size();
            size_t smaller = has_right && vec[right(index)].t < vec[left(index)].t ? right(index) : left(index);

            if (vec[smaller].t < vec[index].t) {
                swap(index, smaller);
            } else {
                break;
            }
            index = smaller;
        }
        return index;
    }

    // Adds an element to the heap.
    // ref_index is 0 if nothing is being referenced.
    void add(T t, size_t ref_index) {
        size_t index = vec.size();
        vec.push_back(Element { t, ref_index });
        if constexpr (!Standalone)
            ref->set_ref_index(ref_index, index);

        heap_up(index);
    }

    // Removes the element at a specific index.
    void remove(size_t i) {
        if (i != 0) {
            if constexpr (!Standalone) {
                ref->set_ref_index(vec[vec.size() - 1].ref_index, i);
                ref->set_ref_index(vec[i].ref_index, 0); // Untrack this
            }
            vec[i] = vec[vec.size() - 1];
            vec.pop_back();

            // Heap down
            size_t index = heap_down(i);
            if (index == i)
                heap_up(index);
        }
    }

    // Gets the minimum element and removes it if it exists
    std::optional<T> remove_min() {
        std::optional<T> min_elem = min();
        if (min_elem.has_value()) {
            remove(root());
        }

        return min_elem;
    }
};

template<typename T>
struct KineticHeap {
    MinHeap<MovingObject<T>, double> items;
    // Each node (except the root) has a certificate comparing it to its parent.
    MinHeap<double, MovingObject<T> > certificates;
    int time;

    KineticHeap(std::vector<MovingObject<T> > items_) : items(&certificates), certificates(&items), time(0) {
        for (MovingObject<T> item_ : items_) {
            item_.curtime = &time;
            items.add(item_, 0);
        }

        for (size_t i = items.left(items.root()); i < items.vec.size(); ++i) {
            maybeAddCertificate(i, time);
        }
    }

    // Potentially add a certificate comparing element i and its parent.
    // No certificate gets added if they're moving away from each other.
    void maybeAddCertificate(size_t i, double time) {
        MovingObject<T>& item = items.vec[i].t;
        MovingObject<T>& parent = items.vec[items.parent(i)].t;
        double intersection = item.getIntersectionTime(parent);
        if (intersection > time || intersection == time && item.velocity < parent.velocity)
            certificates.add(intersection, i);
    }

    std::optional<MovingObject<T>> min() {
        return items.min();
    }

    void fastforward(int timeToForward) {
        time += timeToForward;

        while (certificates.min().value_or(std::numeric_limits<double>::infinity()) < time) {
            double time = certificates.min().value();
            size_t swap_i = certificates.min_ref_index().value();
            size_t parent = items.parent(swap_i); // must exist since the root node has no certificate

            // Up to 5 certificates need to be invalidated.
            certificates.remove_min();
            certificates.remove(items.ref_index(parent));
            if (items.sibling(swap_i) < items.vec.size()) {
                certificates.remove(items.ref_index(items.sibling(swap_i)));
                // Can't exist if sibling doesn't
                if (items.left(swap_i) < items.vec.size()) {
                    certificates.remove(items.ref_index(items.left(swap_i)));
                    // Can't exist if left child doesn't
                    if (items.right(swap_i) < items.vec.size())
                        certificates.remove(items.ref_index(items.right(swap_i)));
                }
            }

            items.swap(swap_i, parent);

            // Up to 4 certificates need to be added.
            // The new node at index swap_i does not get a certificate since it just overtook its now-parent.
            if (parent != items.root())
                maybeAddCertificate(parent, time);
            if (items.sibling(swap_i) < items.vec.size()) {
                maybeAddCertificate(items.sibling(swap_i), time);
                // Can't exist if sibling doesn't
                if (items.left(swap_i) < items.vec.size()) {
                    maybeAddCertificate(items.left(swap_i), time);
                    // Can't exist if left child doesn't
                    if (items.right(swap_i) < items.vec.size())
                        maybeAddCertificate(items.right(swap_i), time);
                }
            }
        }
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const KineticHeap<T>& heap) {
    out << "Item: ";
    for (int i = heap.items.root(); i < heap.items.vec.size(); ++i)
        out << "(" << heap.items.vec[i].t.value << ", " << heap.items.ref_index(i) << "), ";
    out << "\nCert: ";
    for (int i = heap.certificates.root(); i < heap.certificates.vec.size(); ++i)
        out << "(" << heap.certificates.vec[i].t << ", " << heap.certificates.ref_index(i) << "), ";
    out << "\n";
    return out;
}
