#include "rail.hpp"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

int checks = 0;

void check(bool condition) {
    ++checks;

    if (!condition) {
        throw std::runtime_error(
            "Check failed: " + std::to_string(checks)
        );
    }
}

template<class F>
void rejects(F operation) {
    bool threw = false;

    try {
        operation();
    } catch (const std::exception&) {
        threw = true;
    }

    check(threw);
}

int main() {
    try {
        // Valid times.
        check(rail::parseTime("23:59") == 1439);
        check(rail::parseTime("00:00") == 0);

        // Invalid times must be rejected.
        for (const auto& invalid : {
            "24:00",
            "12:60",
            "9:00",
            "12:0x",
            "-1:00",
            "00:00x"
        }) {
            rejects([&] {
                rail::parseTime(invalid);
            });
        }

        // Arrival after midnight.
        check(
            rail::formatTime(1445) == "00:05 (+1d)"
        );

        rail::Timetable timetable({
            {"A", "X", "Y", 480, 500},
            {"B", "Y", "Z", 505, 530},
            {"C", "Y", "Z", 504, 520},
            {"D", "X", "Z", 490, 540}
        });

        const auto results =
            timetable.search("X", "Z", 480);

        // One direct route and one valid transfer.
        check(results.size() == 2);
        check(results.at(0).legs.size() == 2);
        check(results.at(0).duration() == 50);

        // Train C leaves too early for a five-minute transfer.
        check(
            results.at(0).legs.back().train == "B"
        );

        // No departure after the requested time.
        check(
            timetable.search("X", "Z", 491).empty()
        );

        // Reverse routes are not assumed.
        check(
            timetable.search("Z", "X", 0).empty()
        );

        rejects([&] {
            timetable.search("unknown", "Z", 0);
        });

        rejects([&] {
            timetable.search("X", "X", 0);
        });

        rejects([&] {
            timetable.search("X", "Z", -1);
        });

        rejects([&] {
            timetable.search("X", "Z", 0, -1);
        });

        // Zero-duration services are invalid.
        rejects([] {
            rail::Timetable invalid({
                {"A", "X", "Y", 500, 500}
            });
        });

        const auto demo =
            rail::Timetable::load("data/timetable.csv");

        const auto night = demo.search(
            "Leverkusen Mitte",
            "Koeln Hbf",
            1400
        );

        check(night.size() == 1);
        check(night.at(0).duration() == 25);

        check(
            night.at(0).legs.back().arrival == 1445
        );

        rejects([] {
            rail::Timetable::load(
                "missing-file.csv"
            );
        });

        // Malformed CSV records must be rejected.
        const std::string path =
            "invalid_test_fixture.csv";

        for (const auto& row : {
            "T,X,Y,08:00,09:00,extra",
            "T,X,Y,08:00,",
            "T,X,Y,24:00,09:00",
            "T,X,X,08:00,09:00"
        }) {
            {
                std::ofstream file(path);

                if (!file) {
                    throw std::runtime_error(
                        "Cannot create test fixture"
                    );
                }

                file
                    << "train,from,to,departure,arrival\n"
                    << row << '\n';
            }

            rejects([&] {
                rail::Timetable::load(path);
            });
        }

        std::remove(path.c_str());

        std::cout
            << checks << " checks passed\n";

        return 0;

    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
