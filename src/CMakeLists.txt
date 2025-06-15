set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(MSVC)
    add_definitions(-D_WIN32_WINNT=0x600)
endif()

include(FetchContent)
FetchContent_Declare(
        grpc
        GIT_REPOSITORY https://github.com/grpc/grpc.git
        GIT_TAG v1.51.x
)
FetchContent_MakeAvailable(grpc)

set(_PROTOBUF_LIBPROTOBUF libprotobuf)
set(_REFLECTION grpc++_reflection)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
set(_GRPC_GRPCPP grpc++)
if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
endif()

get_filename_component(rng_proto "rng.proto" ABSOLUTE)
get_filename_component(rng_proto_path "${rng_proto}" PATH)

set(rng_proto_sources "${CMAKE_CURRENT_BINARY_DIR}/rng.pb.cc")
set(rng_proto_headers "${CMAKE_CURRENT_BINARY_DIR}/rng.pb.h")
set(rng_grpc_sources "${CMAKE_CURRENT_BINARY_DIR}/rng.grpc.pb.cc")
set(rng_grpc_headers "${CMAKE_CURRENT_BINARY_DIR}/rng.grpc.pb.h")
add_custom_command(
        OUTPUT "${rng_proto_sources}" "${rng_proto_headers}" "${rng_grpc_sources}" "${rng_grpc_headers}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${rng_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${rng_proto}"
        DEPENDS "${rng_proto}")

get_filename_component(health_proto "health.proto" ABSOLUTE)
get_filename_component(health_proto_path "${health_proto}" PATH)

set(health_proto_sources "${CMAKE_CURRENT_BINARY_DIR}/health.pb.cc")
set(health_proto_headers "${CMAKE_CURRENT_BINARY_DIR}/health.pb.h")
set(health_grpc_sources "${CMAKE_CURRENT_BINARY_DIR}/health.grpc.pb.cc")
set(health_grpc_headers "${CMAKE_CURRENT_BINARY_DIR}/health.grpc.pb.h")
add_custom_command(
        OUTPUT "${health_proto_sources}" "${health_proto_headers}" "${health_grpc_sources}" "${health_grpc_headers}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${health_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${health_proto}"
        DEPENDS "${health_proto}")

add_library(rng_grpc_proto
        ${rng_grpc_sources}
        ${rng_grpc_headers}
        ${rng_proto_sources}
        ${rng_proto_headers})
target_link_libraries(rng_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})

add_library(health_grpc_proto
        ${health_grpc_sources}
        ${health_grpc_headers}
        ${health_proto_sources}
        ${health_proto_headers})
target_link_libraries(health_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})

add_library(psirngclient SHARED psirngclient.cpp)
add_library(psirngclient_static psirngclient.cpp)

target_link_libraries(psirngclient
        rng_grpc_proto
        health_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})

target_link_libraries(psirngclient_static
        rng_grpc_proto
        health_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})

target_include_directories(psirngclient PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(psirngclient_static PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

set(BORINGSSL_INCLUDE_DIR ${CMAKE_BINARY_DIR}/_deps/grpc-src/third_party/boringssl-with-bazel/src/include)

target_include_directories(psirngclient PUBLIC ${BORINGSSL_INCLUDE_DIR})
target_include_directories(psirngclient_static PUBLIC ${BORINGSSL_INCLUDE_DIR})

if (CMAKE_GENERATOR MATCHES "Makefiles")
    set_target_properties(psirngclient_static PROPERTIES OUTPUT_NAME psirngclient)
endif ()

install(TARGETS psirngclient psirngclient_static
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(FILES psirngclient.h DESTINATION include)