#include <vector>
#include <utility>
#include <experimental/optional>
#include <set>
#include <map>

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

    double getPosition() const {
        return initialPosition + velocity * (*curtime);
    }

    bool operator<(const MovingObject &other) const {
        return getPosition() < other.getPosition();
    }
};

template<typename T>
struct KineticSuccessor {
    std::vector<MovingObject<T> > items;
    std::set<std::pair<double, std::pair<MovingObject<T>, MovingObject<T> > > > certificates;
    std::map<MovingObject<T>, int> arrayLocations;
    int *time;

    std::pair<double, std::pair<MovingObject<T>, MovingObject<T> > > getCertificate(MovingObject<T> a, MovingObject<T> b) {
        double intersectionTime = a.getIntersectionTime(b);

        if (b.velocity > a.velocity) { // they have already crossed each other
            intersectionTime = -std::numeric_limits<double>::infinity();
        }
        return make_pair(intersectionTime, make_pair(a, b));
    }

    void insertCertificate(MovingObject<T> a, MovingObject<T> b) {
        auto certificate = getCertificate(a, b);
        if (certificate.first != -std::numeric_limits<double>::infinity()) {
            certificates.insert(certificate);
        }
    }

    // must have at least one element
    KineticSuccessor(std::vector<MovingObject<T> > itemsUnsorted, int *t) {
        items = itemsUnsorted;
        time = t;

        items = sort(items.begin(), items.end());

        for (int i = 0; i < items.size() - 1; i++) {
            insertCertificate(items[i], items[i + 1]);
        }
    }

    int findLocation(MovingObject<T> m) {
        return (arrayLocations.find(m) != arrayLocations.end() ? arrayLocations[m] : -1);
    }

    std::optional<MovingObject<T>> findSuccessor(MovingObject<T> m) {
        return (findLocation(m) + 1 < items.size() ? std::make_optional(items[findLocation(m) + 1]) : std::nullopt);
    }

    void fastforward(int timeToForward) {
        *time += timeToForward;

        auto cur = *certificates.begin();
        while (cur.first < time) {
            // swap them
            MovingObject<T> firstObject = cur.second.first;
            int firstLocation = arrayLocations[firstObject];

            if (firstLocation > 0) {
                certificates.erase(getCertificate(arrayLocations[firstLocation - 1], arrayLocations[firstLocation]));
            }

            if (firstLocation + 1 < items.size() - 1) {
                certificates.erase(getCertificate(arrayLocations[firstLocation + 1], arrayLocations[firstLocation + 2]));
            }

            std::swap(arrayLocations[firstLocation], arrayLocations[firstLocation + 1]);

            if (firstLocation > 0) {
                insertCertificate(arrayLocations[firstLocation - 1], arrayLocations[firstLocation]);
            }

            if (firstLocation + 1 < items.size() - 1) {
                insertCertificate(arrayLocations[firstLocation + 1], arrayLocations[firstLocation + 2]);
            }

            // don't reinsert our cross into the certificates set, because they can't cross back

            certificates.erase(certificates.begin());

            if (certificates.size() > 0) {
                cur = *certificates.begin();
            } else {
                break;
            }

        }
    }



};
