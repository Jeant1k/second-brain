#pragma once

#include <boost/uuid/uuid_io.hpp>
#include <string>
#include <unordered_map>
#include <userver/storages/postgres/io/enum_types.hpp>
#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/supported_types.hpp>
#include <userver/utils/strong_typedef.hpp>
#include <vector>

#include "docs/yaml/definitions.hpp"

namespace current_actions::models {

using TaskId = userver::utils::StrongTypedef<class TaskIdTag, boost::uuids::uuid>;
using UserId = userver::utils::StrongTypedef<class UserIdTag, std::int64_t>;

enum class Priority {
    kHigh,
    kMedium,
    kLow
};

enum class Status {
    kActive,
    kCompleted
};

struct TaskInfo {
    UserId user_id;
    std::string description;
    std::optional<boost::uuids::uuid> project_id;
    Priority priority;
    std::vector<std::string> tags;
};

struct FullTaskInfo : public TaskInfo {
    TaskId id;
    Status status;
    userver::storages::postgres::TimePointTz created_at;
    userver::storages::postgres::TimePointTz updated_at;
    std::optional<userver::storages::postgres::TimePointTz> completed_at;
};

const std::unordered_map<handlers::Priority, Priority> kMapPriority = {
    {handlers::Priority::kLow, Priority::kLow},
    {handlers::Priority::kMedium, Priority::kMedium},
    {handlers::Priority::kHigh, Priority::kHigh},
};

struct Cursor {
    userver::storages::postgres::TimePointTz updated_at;
    TaskId id;
};

Cursor DeserializeCursorFromString(const std::string& cursor);

std::string SerializeCursorToString(const Cursor& cursor);

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
    static constexpr EnumeratorList enumerators{
        {EnumType::kActive, "active"},
        {EnumType::kCompleted, "completed"}
    };
};

}  // namespace userver::storages::postgres::io
