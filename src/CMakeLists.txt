find_package(SQLite3 REQUIRED)
add_executable(${CMAKE_PROJECT_NAME} main.cpp BatteryReader.cpp BatteryDatabase.cpp)
target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_20)
target_link_libraries(${CMAKE_PROJECT_NAME} SQLite::SQLite3)

install(TARGETS ${CMAKE_PROJECT_NAME}
	DESTINATION ${CMAKE_INSTALL_BINDIR})
