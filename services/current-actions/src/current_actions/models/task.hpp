#pragma once

#include <string>
#include <optional>
#include <vector>
#include <unordered_map>

#include <userver/storages/postgres/io/enum_types.hpp>
#include <userver/storages/postgres/io/io_fwd.hpp>

#include "docs/yaml/definitions.hpp"

namespace current_actions::models {

enum class Priority {
    kHigh,
    kMedium,
    kLow,
};

struct Task {
    std::int64_t user_id;
    std::string description;
    std::optional<std::string> project_id;
    Priority priority;
    std::vector<std::string> tags;
};

const std::unordered_map<current_actions::handlers::Priority, Priority> kMapPriority = {
    {current_actions::handlers::Priority::kLow, Priority::kLow},
    {current_actions::handlers::Priority::kMedium, Priority::kMedium},
    {current_actions::handlers::Priority::kHigh, Priority::kHigh},
};    
      
}  // current_actions::models

namespace userver::storages::postgres::io {

template <>
struct CppToUserPg<current_actions::models::Priority>
    : EnumMappingBase<current_actions::models::Priority> {
    static constexpr DBTypeName postgres_name = "current_actions.tasks.priority";
    static constexpr EnumeratorList enumerators{
        {EnumType::kHigh, "high"},
        {EnumType::kMedium, "medium"},
        {EnumType::kLow, "low"}};
};
    
}  // namespace userver::storages::postgres::io
    