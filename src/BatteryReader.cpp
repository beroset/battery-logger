/*
 * Copyright (C) 2023,2024 - Ed Beroset <beroset@ieee.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "BatteryReader.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

// given a path, returns the first line of that file if it exists
[[nodiscard]] std::optional<std::string> readFileValue(fs::path valuefile) {
    std::ifstream vf{valuefile};
    std::string result;
    if (std::getline(vf, result)) {
        return result;
    }
    return {};
}

/*
 * 1. for each dir under /sys/class/power_supply
 * 2. if there is a file `present`
 * 3. and a file `voltage_now`
 * 4. and the value in `present` is `1`
 * 5. and the type is "Battery"
 * 6. then return the path
 */
[[nodiscard]] std::optional<fs::path> BatteryReader::findBatteryDevicePath() {
    fs::path basedir{"/sys/class/power_supply"};
    if (fs::exists(basedir)) {
        for (auto const& dir_entry : std::filesystem::directory_iterator{basedir}) {
            if (readFileValue(dir_entry.path() / "present").value_or("x") == "1" &&
                readFileValue(dir_entry.path() / "type").value_or("x") == "Battery") {
                auto voltage_now{readFileValue(dir_entry.path() / "voltage_now")};
                if (voltage_now)
                    return dir_entry.path();
            }
        }
    }
    return {};
}

BatteryReader::BatteryReader() {
    /*
     * Create a BatteryReader or throw if no battery detected
     */
    auto bp{findBatteryDevicePath()};
    if (!bp) {
        throw std::runtime_error{"Did not find battery"};
    }
    batteryPath = *bp;
}

[[nodiscard]] std::string BatteryReader::getSQL() {
    auto voltage{readFileValue(batteryPath / "voltage_now").value_or("0.0")};
    auto current{readFileValue(batteryPath / "current_now").value_or("0.0")};
    auto capacity{readFileValue(batteryPath / "capacity").value_or("0.0")};
    auto temp{readFileValue(batteryPath / "temp").value_or("0.0")};
    auto status{readFileValue(batteryPath / "status").value_or("Unknown")};

    std::stringstream sql{};
    sql << "INSERT INTO battery VALUES ( CURRENT_TIMESTAMP"
        << ", " << voltage
        << ", " << current
        << ", " << capacity
        << ", " << temp
        << ", \"" << status
        << "\", \"" << batteryPath.string()
        << "\" )";
    return sql.str();
}
