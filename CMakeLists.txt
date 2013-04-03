project(netmon)

add_executable(netmon main.cpp)

install(TARGETS netmon RUNTIME DESTINATION bin)
