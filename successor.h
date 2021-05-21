#include <vector>
#include <utility>
#include <experimental/optional>
#include <set>
#include <unordered_map>
#include <iostream>
#include <algorithm>

template<typename T>
struct MovingObject {
    int initialPosition;
    int velocity;
    int *curtime;
    T value;

    // Constructor used as the default constructor for the kinetic heap
    MovingObject() {}

    // Constructor used for kinetic heap, since
    // `curtime` is assigned at construction there
    MovingObject(int ip, int v, T val) : initialPosition(ip), velocity(v), curtime(nullptr), value(val) {}

    MovingObject(int ip, int v, int *t, T val) : initialPosition(ip), velocity(v), curtime(t), value(val) {}

    double getIntersectionTime(const MovingObject &other) const {
        // Avoid returning NaN when both numerator and denominator are 0
        if (velocity - other.velocity == 0)
            return -std::numeric_limits<double>::infinity();
        return (other.initialPosition - initialPosition * 1.0) / (velocity - other.velocity);
    }

    int getPosition() const {
        return initialPosition + velocity * (*curtime);
    }

    bool operator<(const MovingObject &other) const {
        if (getPosition() == other.getPosition()) {
            return value < other.value;
        }
        return getPosition() < other.getPosition();
    }

    bool operator==(const MovingObject &other) const {
        return initialPosition == other.initialPosition && velocity == other.velocity && value == other.value;
    }

};

template<typename T>
struct ObjectHasher {
    int operator()(const MovingObject<T> &key) const {
        return key.initialPosition * 31 + key.velocity;
    }
};

template<typename T>
struct KineticSuccessor {
    std::vector<MovingObject<T>> items;
    std::set<std::pair<double, std::pair<MovingObject<T>, MovingObject<T>>>> certificates;
    std::unordered_map<MovingObject<T>, int, ObjectHasher<T>> arrayLocations;
    int *time;

    std::pair<double, std::pair<MovingObject<T>, MovingObject<T> > > getCertificate(MovingObject<T> a, MovingObject<T> b) {
        double intersectionTime = a.getIntersectionTime(b);

        if (b.velocity > a.velocity) { // they have already crossed each other
            intersectionTime = -std::numeric_limits<double>::infinity();
        }
        return {intersectionTime, {a, b}};
    }

    void insertCertificate(MovingObject<T> a, MovingObject<T> b) {
        auto certificate = getCertificate(a, b);
        //std::cout << "inserting cert for " << a.value << " " << b.value << " time " << certificate.first << std::endl;
        if (certificate.first != -std::numeric_limits<double>::infinity()) {

            //std::cout << " certs size " << certificates.size() << " item " << (*certificates.begin()).second.first.value << ' ' << (*certificates.begin()).second.second.value <<  std::endl;
            //std::cout << "inserted " << std::endl;
            certificates.insert(certificate);
            //std::cout << " certs size " << certificates.size() << " item " << (*certificates.begin()).second.first.value << ' ' << (*certificates.begin()).second.second.value <<  std::endl;
            
        }
    }

    // must have at least one element
    KineticSuccessor(std::vector<MovingObject<T> > itemsUnsorted, int *t) {
        items = itemsUnsorted;
        time = t;

        sort(items.begin(), items.end());

        for (int i = 0; i < items.size() - 1; i++) {
            insertCertificate(items[i], items[i + 1]);
        }

        for (int i = 0; i < items.size(); i++) {
            arrayLocations[items[i]] = i;
        }
    }

    int findLocation(MovingObject<T> m) {
        return (arrayLocations.find(m) != arrayLocations.end() ? arrayLocations[m] : -1);
    }

    std::optional<MovingObject<T>> findSuccessor(MovingObject<T> m) {
        return (findLocation(m) + 1 < items.size() ? std::make_optional(items[findLocation(m) + 1]) : std::nullopt);
    }

    //int counter = 0;

    void fastforward(int timeToForward) {
        *time += timeToForward;

        if (certificates.size() == 0) return;

        auto cur = *certificates.begin();
        auto curit = certificates.begin();
        while (cur.first < *time) {
            // swap them
            MovingObject<T> firstObject = cur.second.first;
            int firstLocation = arrayLocations[firstObject];


            certificates.erase(curit);

            //counter++;

            //if (counter % 1000000 == 0) std::cout << " at " << cur.first << " pos " << firstLocation << " " << firstObject.value << " certs size " << certificates.size() << std::endl;

            //std::cout << " certs size " << certificates.size() <<  std::endl;
            //for (auto i : items) {
            //    std::cout << i.initialPosition << ' ' << i.velocity << ' ' << i.value << " | " ;
            //}
            //std::cout << std::endl;
            if (firstLocation > 0) {
                certificates.erase(getCertificate(items[firstLocation - 1], items[firstLocation]));
            }

            if (firstLocation + 1 < items.size() - 1) {
                certificates.erase(getCertificate(items[firstLocation + 1], items[firstLocation + 2]));
            }

            //auto tmp = items[firstLocation];
            //items[firstLocation] = items[firstLocation + 1];
            //items[firstLocation + 1] = tmp;
            std::swap(items[firstLocation], items[firstLocation + 1]);

            arrayLocations[items[firstLocation]]--;
            arrayLocations[items[firstLocation + 1]]++;

            //std::cout << " certs size " << certificates.size() <<  std::endl;
            if (firstLocation > 0) {
                insertCertificate(items[firstLocation - 1], items[firstLocation]);
            }

            if (firstLocation + 1 < items.size() - 1) {
                insertCertificate(items[firstLocation + 1], items[firstLocation + 2]);
            }


            //std::cout << " certs size " << certificates.size() <<  std::endl;

            // don't reinsert our cross into the certificates set, because they can't cross back

            //std::cout << "done" << std::endl;

            //std::cout << "done" << std::endl;
            //std::cout << "done" << std::endl;
            //for (auto i : items) {
            //    std::cout << i.initialPosition << ' ' << i.velocity << ' ' << i.value << " | " ;
            //}
            //std::cout << std::endl;
            //std::cout << " certs size " << certificates.size() <<  std::endl;
            if (certificates.size() > 0) {
                //std::cout << "yes begin" << std::endl;
                cur = *certificates.begin();
                curit = certificates.begin();
                //std::cout << "acquired " << cur.first << std::endl;
            } else {
                break;
            }


        }
    }



};
