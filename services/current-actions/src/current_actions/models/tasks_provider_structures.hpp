#pragma once

#include <string>

#include <boost/uuid/uuid_io.hpp>

#include <userver/storages/postgres/io/enum_types.hpp>
#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/supported_types.hpp>
#include <userver/utils/strong_typedef.hpp>

#include "docs/yaml/definitions.hpp"

namespace current_actions::models {

using TaskId = userver::utils::StrongTypedef<class TaskIdTag, boost::uuids::uuid>;
using UserId = userver::utils::StrongTypedef<class UserIdTag, std::int64_t>;

enum class Status { kActive, kCompleted, kMovedToSomedayLater, kDeleted };

std::optional<handlers::TaskStatus> Transform(const std::optional<Status> status);
std::optional<Status> Transform(const std::optional<handlers::TaskStatus> status);

struct Task {
    TaskId id;
    UserId user_id;
    std::string name;
    std::string description;
    Status status;
    userver::storages::postgres::TimePointTz created_at;
    userver::storages::postgres::TimePointTz updated_at;
    std::optional<userver::storages::postgres::TimePointTz> completed_at;
};

struct TaskForCreate {
    UserId user_id;
    std::string name;
    std::string description;
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
struct CppToUserPg<current_actions::models::Status> : EnumMappingBase<current_actions::models::Status> {
    static constexpr DBTypeName postgres_name = "current_actions.task_status";
    static constexpr EnumeratorList enumerators{
        {EnumType::kActive, "active"},
        {EnumType::kCompleted, "completed"},
        {EnumType::kMovedToSomedayLater, "moved_to_someday_later"},
        {EnumType::kDeleted, "deleted"}
    };
};

}  // namespace userver::storages::postgres::io
