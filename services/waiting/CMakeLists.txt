cmake_minimum_required(VERSION 3.12...3.31)
project(waiting CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(DownloadUserver)

find_package(userver COMPONENTS core postgresql chaotic QUIET)
if(NOT userver_FOUND)
  # Tries TRY_DIR first, falls back to downloading userver from GitHub using CPM.
  download_userver(TRY_DIR third_party/userver)
endif()

userver_setup_environment()


# YAML Codegen
file(GLOB_RECURSE SCHEMAS ${CMAKE_CURRENT_SOURCE_DIR}/docs/yaml/*.yaml)
userver_target_generate_chaotic(${PROJECT_NAME}-chgen
    GENERATE_SERIALIZERS
    # Map '/components/schemas/*' JSONSchema types to C++ types in 'waiting::handlers' namespace
    LAYOUT
        "/components/schemas/([^/]*)/=waiting::handlers::{0}"
    OUTPUT_DIR
        ${CMAKE_CURRENT_BINARY_DIR}/src
    SCHEMAS
        ${SCHEMAS}
    RELATIVE_TO
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# SQL Codegen
userver_add_sql_library(
    ${PROJECT_NAME}_sql
    NAMESPACE waiting
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
    SQL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/waiting/database/sql/*.sql
)


# Common sources
add_library(${PROJECT_NAME}_objs OBJECT
    # views
    src/views/contract/models/responses.hpp
    src/views/models/responses.cpp
    src/views/contract/base_handler.hpp
    src/views/waiting/v1/task/view.hpp
    src/views/waiting/v1/task/view.cpp
    src/views/waiting/v1/task/complete/view.hpp
    src/views/waiting/v1/task/complete/view.cpp
    src/views/waiting/v1/task/reactivate/view.hpp
    src/views/waiting/v1/task/reactivate/view.cpp
    src/views/waiting/v1/task/list/view.hpp
    src/views/waiting/v1/task/list/view.cpp
    src/views/waiting/v1/task/update/view.hpp
    src/views/waiting/v1/task/update/view.cpp
    src/views/waiting/v1/task/remove/view.hpp
    src/views/waiting/v1/task/remove/view.cpp

    # waiting
    src/waiting/contract/models/exceptions.hpp
    src/waiting/contract/managers/tasks_manager.hpp
    src/waiting/contract/models/tasks_provider_structures.hpp
    src/waiting/managers/tasks_manager.cpp
    src/waiting/models/tasks_provider_structures.cpp
    src/waiting/providers/tasks_provider/tasks_provider.hpp
    src/waiting/providers/tasks_provider/tasks_provider.cpp
)

target_link_libraries(${PROJECT_NAME}_objs PUBLIC
    userver::postgresql
    ${PROJECT_NAME}-chgen # Зависимость от YAML codegen
    ${PROJECT_NAME}_sql   # Зависимость от SQL codegen
)


# The Service
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)


# Unit Tests
# add_executable(${PROJECT_NAME}_unittest
#     ...
# )
# target_link_libraries(${PROJECT_NAME}_unittest PRIVATE ${PROJECT_NAME}_objs userver::utest)
# add_google_tests(${PROJECT_NAME}_unittest)


# Benchmarks
# add_executable(${PROJECT_NAME}_benchmark
#     ...
# )
# target_link_libraries(${PROJECT_NAME}_benchmark PRIVATE ${PROJECT_NAME}_objs userver::ubench)
# add_google_benchmark_tests(${PROJECT_NAME}_benchmark)


# Functional Tests
userver_testsuite_add_simple()


# Install
include(GNUInstallDirs)

if(DEFINED ENV{PREFIX})
  message(STATUS "Set install prefix: $ENV{PREFIX}")
  file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
  set(CMAKE_INSTALL_PREFIX ${PREFIX_PATH})
endif()

file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})
