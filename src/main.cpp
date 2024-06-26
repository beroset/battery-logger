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
#include "BatteryDatabase.h"
#include <filesystem>
#include <iostream>

int main(int argc, char *argv[])
{
    namespace fs = std::filesystem;
    try {
        BatteryReader batt;
        fs::path dbfilename{"/home/ceres/battery.db"};
        bool create{!fs::exists(dbfilename)};
        BatteryDatabase db{dbfilename};
        if (create) {
            db.apply(batt.create_string);
        }
        db.apply(batt.getSQL());
    }
    catch (std::runtime_error &err) {
        std::cerr << err.what() << '\n';
        return 1;
    }
}
