#include "rail.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char** argv) {
    try {
        if (argc == 2 && std::string(argv[1]) == "--help") {
            std::cout
                << "Usage: railconnect [timetable.csv [FROM TO HH:MM]]\n"
                << "Without a query, starts an interactive search.\n"
                << "Use --stations after CSV to list stations.\n";
            return 0;
        }

        if (!(argc == 1 || argc == 2 || argc == 3 || argc == 5)) {
            throw std::invalid_argument("Use --help for usage");
        }

        const auto timetable = rail::Timetable::load(
            argc > 1 ? argv[1] : "data/timetable.csv"
        );

        if (argc == 3 && std::string(argv[2]) != "--stations") {
            throw std::invalid_argument(
                "Use --stations or provide FROM TO HH:MM"
            );
        }

        std::cout
            << "RailConnect | Fictional demonstration timetable\n";

        if (argc != 5) {
            std::cout << "Stations:\n";

            for (const auto& station : timetable.stations()) {
                std::cout << "  " << station << '\n';
            }

            if (argc == 3) {
                return 0;
            }
        }

        std::string from;
        std::string to;
        std::string time;

        if (argc == 5) {
            from = argv[2];
            to = argv[3];
            time = argv[4];
        } else {
            std::cout << "From: ";
            if (!std::getline(std::cin, from)) return 0;

            std::cout << "To: ";
            if (!std::getline(std::cin, to)) return 0;

            std::cout << "Earliest departure (HH:MM): ";
            if (!std::getline(std::cin, time)) return 0;
        }

        const auto results = timetable.search(
            from, to, rail::parseTime(time)
        );

        if (results.empty()) {
            std::cout
                << "No matching connection in this timetable.\n";
            return 0;
        }

        int index = 0;

        for (const auto& journey : results) {
            std::cout
                << '\n' << ++index << ". "
                << rail::formatTime(journey.legs.front().departure)
                << " -> "
                << rail::formatTime(journey.legs.back().arrival)
                << " | " << journey.duration() << " min"
                << " | changes: " << journey.legs.size() - 1
                << '\n';

            for (const auto& leg : journey.legs) {
                std::cout
                    << "   " << leg.train
                    << " | " << leg.from << " -> " << leg.to
                    << " | " << rail::formatTime(leg.departure)
                    << " - " << rail::formatTime(leg.arrival)
                    << '\n';
            }
        }
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
