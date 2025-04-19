#pragma once

#include <boost/uuid/uuid_io.hpp>
#include <string>
#include <unordered_map>
#include <userver/storages/postgres/io/enum_types.hpp>
#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/utils/strong_typedef.hpp>
#include <vector>

#include "docs/yaml/definitions.hpp"

namespace current_actions::models {

using TaskId = userver::utils::StrongTypedef<class TaskIdTag, boost::uuids::uuid>;

enum class Priority {
    kHigh,
    kMedium,
    kLow,
};

struct Task {
    std::int64_t user_id;
    std::string description;
    std::optional<boost::uuids::uuid> project_id;
    Priority priority;
    std::vector<std::string> tags;
};

const std::unordered_map<current_actions::handlers::Priority, Priority> kMapPriority = {
    {current_actions::handlers::Priority::kLow, Priority::kLow},
    {current_actions::handlers::Priority::kMedium, Priority::kMedium},
    {current_actions::handlers::Priority::kHigh, Priority::kHigh},
};

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

}  // namespace userver::storages::postgres::io
