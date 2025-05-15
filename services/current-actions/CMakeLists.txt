cmake_minimum_required(VERSION 3.12...3.31)
project(current-actions CXX)

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
    # Map '/components/schemas/*' JSONSchema types to C++ types in 'current_actions::handlers' namespace
    LAYOUT
        "/components/schemas/([^/]*)/=current_actions::handlers::{0}"
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
    NAMESPACE current_actions
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
    SQL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/current_actions/database/sql/*.sql
)


# Common sources
add_library(${PROJECT_NAME}_objs OBJECT
    # views
    src/views/contract/models/responses.hpp
    src/views/models/responses.cpp
    src/views/contract/base_handler.hpp
    src/views/current-actions/v1/task/view.hpp
    src/views/current-actions/v1/task/view.cpp
    src/views/current-actions/v1/task/complete/view.hpp
    src/views/current-actions/v1/task/complete/view.cpp
    src/views/current-actions/v1/task/reactivate/view.hpp
    src/views/current-actions/v1/task/reactivate/view.cpp
    src/views/current-actions/v1/task/list/view.hpp
    src/views/current-actions/v1/task/list/view.cpp
    src/views/current-actions/v1/task/update/view.hpp
    src/views/current-actions/v1/task/update/view.cpp
    src/views/current-actions/v1/task/remove/view.hpp
    src/views/current-actions/v1/task/remove/view.cpp
    src/views/current-actions/v1/task/sometime/later/view.hpp
    src/views/current-actions/v1/task/sometime/later/view.cpp
    src/views/internal/current-actions/v1/task/move/view.hpp
    src/views/internal/current-actions/v1/task/move/view.cpp

    # current_actions
    src/current_actions/contract/models/exceptions.hpp
    src/current_actions/contract/managers/tasks_manager.hpp
    src/current_actions/managers/tasks_manager.cpp
    src/current_actions/models/tasks_provider_structures.hpp
    src/current_actions/models/tasks_provider_structures.cpp
    src/current_actions/providers/tasks_provider/tasks_provider.hpp
    src/current_actions/providers/tasks_provider/tasks_provider.cpp

    # clients
    src/clients/contract/base_client.hpp
    src/clients/contract/models/exceptions.hpp
    src/clients/contract/sometime_later/client.hpp
    src/clients/sometime_later/client.hpp
    src/clients/sometime_later/client.cpp
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
