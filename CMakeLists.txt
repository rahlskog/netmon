project(netmon)

add_executable(netmon main.cpp builtins.cpp)
target_link_libraries(netmon rt)

install(TARGETS netmon RUNTIME DESTINATION bin)
