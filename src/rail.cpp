#include "rail.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <set>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace rail {

int parseTime(const std::string& text) {
    if (
        text.size() != 5 ||
        text[2] != ':' ||
        text[0] < '0' || text[0] > '2' ||
        text[1] < '0' || text[1] > '9' ||
        text[3] < '0' || text[3] > '5' ||
        text[4] < '0' || text[4] > '9'
    ) {
        throw std::invalid_argument(
            "Expected a valid time in HH:MM format"
        );
    }

    const int hours =
        (text[0] - '0') * 10 + text[1] - '0';

    if (hours > 23) {
        throw std::invalid_argument(
            "Hour must be between 00 and 23"
        );
    }

    return hours * 60
        + (text[3] - '0') * 10
        + text[4] - '0';
}

std::string formatTime(int minutes) {
    if (minutes < 0) {
        throw std::invalid_argument("Negative time");
    }

    std::ostringstream output;

    output << std::setfill('0')
           << std::setw(2) << (minutes % 1440) / 60
           << ':'
           << std::setw(2) << minutes % 60;

    if (minutes >= 1440) {
        output << " (+" << minutes / 1440 << "d)";
    }

    return output.str();
}

int Journey::duration() const {
    if (legs.empty()) {
        return 0;
    }

    return legs.back().arrival - legs.front().departure;
}

Timetable::Timetable(std::vector<Leg> legs)
    : legs_(std::move(legs)) {

    for (const auto& leg : legs_) {
        if (
            leg.train.empty() ||
            leg.from.empty() ||
            leg.to.empty() ||
            leg.from == leg.to ||
            leg.departure < 0 ||
            leg.departure >= 1440 ||
            leg.arrival <= leg.departure ||
            leg.arrival - leg.departure >= 1440
        ) {
            throw std::invalid_argument(
                "Invalid timetable leg"
            );
        }
    }
}

Timetable Timetable::load(const std::string& path) {
    std::ifstream input(path);

    if (!input) {
        throw std::runtime_error(
            "Cannot open timetable: " + path
        );
    }

    std::string line;

    if (!std::getline(input, line)) {
        throw std::runtime_error("Empty timetable");
    }

    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }

    if (line != "train,from,to,departure,arrival") {
        throw std::runtime_error(
            "Unexpected CSV header"
        );
    }

    std::vector<Leg> legs;
    int lineNumber = 1;

    while (std::getline(input, line)) {
        ++lineNumber;

        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.empty()) {
            continue;
        }

        std::vector<std::string> cells;
        std::size_t start = 0;

        for (;;) {
            const auto end = line.find(',', start);

            cells.push_back(
                line.substr(
                    start,
                    end == std::string::npos
                        ? end
                        : end - start
                )
            );

            if (end == std::string::npos) {
                break;
            }

            start = end + 1;
        }

        try {
            if (cells.size() != 5) {
                throw std::invalid_argument(
                    "Expected five CSV fields"
                );
            }

            const int departure = parseTime(cells[3]);
            int arrival = parseTime(cells[4]);

            // An earlier arrival clock time means the next day.
            if (arrival < departure) {
                arrival += 1440;
            }

            Leg leg{
                cells[0],
                cells[1],
                cells[2],
                departure,
                arrival
            };

            // Validate this record before storing it.
            Timetable validated({leg});

            legs.push_back(std::move(leg));

        } catch (const std::exception& error) {
            throw std::runtime_error(
                "CSV line " + std::to_string(lineNumber)
                + ": " + error.what()
            );
        }
    }

    if (input.bad()) {
        throw std::runtime_error(
            "Error reading timetable"
        );
    }

    if (legs.empty()) {
        throw std::runtime_error(
            "Timetable contains no services"
        );
    }

    return Timetable(std::move(legs));
}

std::vector<std::string> Timetable::stations() const {
    std::set<std::string> unique;

    for (const auto& leg : legs_) {
        unique.insert(leg.from);
        unique.insert(leg.to);
    }

    return {unique.begin(), unique.end()};
}

std::vector<Journey> Timetable::search(
    const std::string& from,
    const std::string& to,
    int earliest,
    int minimumTransfer
) const {

    if (
        earliest < 0 ||
        earliest >= 1440 ||
        minimumTransfer < 0 ||
        minimumTransfer > 1440
    ) {
        throw std::invalid_argument(
            "Invalid search time or transfer interval"
        );
    }

    const auto known = stations();

    if (
        from == to ||
        std::find(known.begin(), known.end(), from)
            == known.end() ||
        std::find(known.begin(), known.end(), to)
            == known.end()
    ) {
        throw std::invalid_argument(
            "Choose two different stations "
            "from the station list"
        );
    }

    std::vector<Journey> results;

    for (const auto& first : legs_) {
        if (
            first.from != from ||
            first.departure < earliest
        ) {
            continue;
        }

        // Direct connection.
        if (first.to == to) {
            results.push_back({{first}});
            continue;
        }

        // Connection with one change.
        for (const auto& second : legs_) {
            if (
                second.from == first.to &&
                second.to == to &&
                second.departure >=
                    first.arrival + minimumTransfer
            ) {
                results.push_back({{first, second}});
            }
        }
    }

    std::stable_sort(
        results.begin(),
        results.end(),
        [](const Journey& a, const Journey& b) {
            if (
                a.legs.back().arrival !=
                b.legs.back().arrival
            ) {
                return a.legs.back().arrival
                    < b.legs.back().arrival;
            }

            if (a.duration() != b.duration()) {
                return a.duration() < b.duration();
            }

            return a.legs.size() < b.legs.size();
        }
    );

    return results;
}

}
