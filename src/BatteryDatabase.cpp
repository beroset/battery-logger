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

#include "BatteryDatabase.h"
#include <iostream>

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    for (int i = 0; i < argc; i++) {
        std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << '\n';
    }
    std::cout << '\n';
    return 0;
}

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

bool BatteryDatabase::apply(const std::string_view sql) {
    char *zErrMsg{nullptr};
    int rc = sqlite3_exec(db, sql.data(), callback, 0, &zErrMsg);
    bool retval{true};  // be optimistic
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << zErrMsg << '\n';
        sqlite3_free(zErrMsg);
        retval = false;
    }
    return retval;
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
    return retval;
}
