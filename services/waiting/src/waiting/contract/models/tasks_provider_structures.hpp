#pragma once

#include <string>

#include <boost/uuid/uuid_io.hpp>

#include <userver/storages/postgres/io/enum_types.hpp>
#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/supported_types.hpp>
#include <userver/utils/strong_typedef.hpp>

#include "docs/yaml/definitions.hpp"

namespace waiting::contract::models {

using TaskId = userver::utils::StrongTypedef<class TaskIdTag, boost::uuids::uuid>;
using UserId = userver::utils::StrongTypedef<class UserIdTag, std::int64_t>;

enum class Status { kActive, kCompleted, kDeleted };

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

struct TaskForUpdate {
    TaskId task_id;
    std::optional<std::string> name;
    std::optional<std::string> description;
};

struct Cursor {
    userver::storages::postgres::TimePointTz updated_at;
    TaskId id;
};

std::optional<Cursor> DeserializeCursorFromString(const std::optional<std::string>& cursor);

std::optional<std::string> SerializeCursorToString(const std::optional<Cursor>& cursor);

}  // namespace waiting::contract::models

namespace userver::storages::postgres::io {

template <>
struct CppToUserPg<waiting::contract::models::Status> : EnumMappingBase<waiting::contract::models::Status> {
    static constexpr DBTypeName postgres_name = "waiting.task_status";
    static constexpr EnumeratorList enumerators{
        {EnumType::kActive, "active"},
        {EnumType::kCompleted, "completed"},
        {EnumType::kDeleted, "deleted"}
    };
};

}  // namespace userver::storages::postgres::io
