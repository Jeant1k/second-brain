#pragma once

#include <string>
#include <vector>

#include <boost/uuid/uuid_io.hpp>

#include <userver/storages/postgres/io/enum_types.hpp>
#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/supported_types.hpp>
#include <userver/utils/strong_typedef.hpp>

#include "docs/yaml/definitions.hpp"

namespace current_actions::models {

using TaskId = userver::utils::StrongTypedef<class TaskIdTag, boost::uuids::uuid>;
using UserId = userver::utils::StrongTypedef<class UserIdTag, std::int64_t>;

enum class Priority { kHigh, kMedium, kLow };

std::optional<handlers::Priority> Transform(const std::optional<Priority> priority);
std::optional<Priority> Transform(const std::optional<handlers::Priority> priority);

enum class Status { kActive, kCompleted };

std::optional<handlers::TaskStatus> Transform(const std::optional<Status> status);
std::optional<Status> Transform(const std::optional<handlers::TaskStatus> status);

struct Tag {
    boost::uuids::uuid id;
    std::string name;
    userver::storages::postgres::TimePointTz created_at;
};

struct Task {
    TaskId id;
    UserId user_id;
    std::string description;
    Status status;
    std::optional<boost::uuids::uuid> project_id;
    Priority priority;
    userver::storages::postgres::TimePointTz created_at;
    userver::storages::postgres::TimePointTz updated_at;
    std::optional<userver::storages::postgres::TimePointTz> completed_at;

    std::vector<Tag> tags;
};

struct TaskForCreate {
    UserId user_id;
    std::string description;
    std::optional<boost::uuids::uuid> project_id;
    std::optional<Priority> priority;
    std::vector<std::string> tags;
};

struct Cursor {
    userver::storages::postgres::TimePointTz updated_at;
    TaskId id;
};

std::optional<Cursor> DeserializeCursorFromString(const std::optional<std::string>& cursor);

std::optional<std::string> SerializeCursorToString(const std::optional<Cursor>& cursor);

}  // namespace current_actions::models

namespace userver::storages::postgres::io {

template <>
struct CppToUserPg<current_actions::models::Priority> : EnumMappingBase<current_actions::models::Priority> {
    static constexpr DBTypeName postgres_name = "current_actions.task_priority";
    static constexpr EnumeratorList enumerators{
        {EnumType::kHigh, "high"},
        {EnumType::kMedium, "medium"},
        {EnumType::kLow, "low"}
    };
};

template <>
struct CppToUserPg<current_actions::models::Status> : EnumMappingBase<current_actions::models::Status> {
    static constexpr DBTypeName postgres_name = "current_actions.task_status";
    static constexpr EnumeratorList enumerators{{EnumType::kActive, "active"}, {EnumType::kCompleted, "completed"}};
};

}  // namespace userver::storages::postgres::io
