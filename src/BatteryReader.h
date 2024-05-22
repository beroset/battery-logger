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
#ifndef BATTERYREADER_H
#define BATTERYREADER_H
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

class BatteryReader {
public:
    BatteryReader();
    static constexpr std::string_view create_string{R"(CREATE TABLE "battery" ( "time" datetime, "voltage" REAL, "current" REAL, "capacity" REAL, "temp" REAL, "status" STRING, "path" STRING ) )"};
    [[nodiscard]] std::string getSQL();
private:
    [[nodiscard]] std::optional<std::filesystem::path> findBatteryDevicePath();
    std::filesystem::path batteryPath;
};
#endif // BATTERYREADER_H
