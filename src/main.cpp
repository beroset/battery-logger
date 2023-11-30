/*
 * Copyright (C) 2023 - Ed Beroset <beroset@ieee.org>
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

#include <fstream>
#include <filesystem>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <sqlite3.h>

namespace fs = std::filesystem;

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    for (int i = 0; i < argc; i++) {
        std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << '\n';
    }
    std::cout << '\n';
    return 0;
}

class BatteryDatabase {
public:
    BatteryDatabase(const std::string& filename);
    ~BatteryDatabase();
    bool apply(const std::string& sql);
private:
    sqlite3 *db;
};

BatteryDatabase::BatteryDatabase(const std::string& filename) {
    int rc = sqlite3_open(filename.c_str(), &db);
    if (rc) {
        sqlite3_close(db);
        throw(std::runtime_error(sqlite3_errmsg(db))); 
    }
}

BatteryDatabase::~BatteryDatabase() {
    sqlite3_close(db);
}

bool BatteryDatabase::apply(const std::string& sql) {
    char *zErrMsg{nullptr};
    int rc = sqlite3_exec(db, sql.data(), callback, 0, &zErrMsg);
    bool retval{true};  // be optimistic
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << zErrMsg << '\n';
        sqlite3_free(zErrMsg);
        retval = false;
    }
    return true;
}

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
 * 2. see if there is a file `present` 
 * 3. and a file `voltage_now`
 * 4. and the value in `present` is `1`
 * 5. and the type is "Battery"
 */
[[nodiscard]] std::optional<fs::path> findBatteryDevicePath() {
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


int main(int argc, char *argv[])
{
    /*
     * 1. for each dir under /sys/class/power_supply
     * 2. see if there is a file `present` 
     * 3. and a file `voltage_now`
     * 4. and the value in `present` is `1`
     * 5. and the type is "Battery"
     * 6. read voltage_now, current_now, temp, and capacity
     * 7. store them with timestamp in the database
     */
    auto batteryPath{findBatteryDevicePath()};
    if (!batteryPath) {
        std::cerr << "Did not find battery\n";
        return 1;
    }

    fs::path dbfilename{"/home/ceres/battery.db"};
    bool create{!fs::exists(dbfilename)};
    BatteryDatabase db{dbfilename};
    if (create) {
        db.apply(R"(CREATE TABLE "battery" ( "time" datetime, "voltage" REAL, "current" REAL, "capacity" REAL, "temp" REAL, "status" STRING, "path" STRING ) )");
    } 

    auto voltage{readFileValue(*batteryPath / "voltage_now").value_or("0.0")};
    auto current{readFileValue(*batteryPath / "current_now").value_or("0.0")};
    auto capacity{readFileValue(*batteryPath / "capacity").value_or("0.0")};
    auto temp{readFileValue(*batteryPath / "temp").value_or("0.0")};
    auto status{readFileValue(*batteryPath / "status").value_or("Unknown")};

    std::stringstream sql{};
    sql << "INSERT INTO battery VALUES ( CURRENT_TIMESTAMP"
        << ", " << voltage
        << ", " << current
        << ", " << capacity
        << ", " << temp
        << ", \"" << status
        << "\", \"" << batteryPath->string()
        << "\" )";
    db.apply(sql.str());
}
