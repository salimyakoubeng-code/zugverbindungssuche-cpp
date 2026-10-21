#pragma once

#include <string>
#include <vector>

namespace rail {

// Convert HH:MM into minutes.
int parseTime(const std::string& text);

// Convert minutes into a readable time.
std::string formatTime(int minutes);

// One train service between two stations.
struct Leg {
    std::string train;
    std::string from;
    std::string to;

    int departure;
    int arrival;
};

// A journey containing one or more legs.
struct Journey {
    std::vector<Leg> legs;

    int duration() const;
};

// Stores the timetable and searches for connections.
class Timetable {
public:
    explicit Timetable(std::vector<Leg> legs);

    static Timetable load(const std::string& path);

    std::vector<std::string> stations() const;

    std::vector<Journey> search(
        const std::string& from,
        const std::string& to,
        int earliest,
        int minimumTransfer = 5
    ) const;

private:
    std::vector<Leg> legs_;
};

}
