include_directories(../src)

add_executable(psirngclient-example main.cpp)

if (NOT MSVC)
    target_link_libraries(psirngclient-example PRIVATE psirngclient)
else()
    target_link_libraries(psirngclient-example PRIVATE psirngclient_static)
endif()