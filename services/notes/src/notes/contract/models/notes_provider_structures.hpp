#pragma once

#include <string>

#include <boost/uuid/uuid_io.hpp>

#include <userver/storages/postgres/io/enum_types.hpp>
#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/supported_types.hpp>
#include <userver/utils/strong_typedef.hpp>

#include "docs/yaml/definitions.hpp"

namespace notes::contract::models {

using NoteId = userver::utils::StrongTypedef<class NoteIdTag, boost::uuids::uuid>;
using UserId = userver::utils::StrongTypedef<class UserIdTag, std::int64_t>;

enum class Status { kActive, kDeleted };

std::optional<handlers::NoteStatus> Transform(const std::optional<Status> status);
std::optional<Status> Transform(const std::optional<handlers::NoteStatus> status);

struct Note {
    NoteId id;
    UserId user_id;
    std::string name;
    std::string description;
    Status status;
    userver::storages::postgres::TimePointTz created_at;
    userver::storages::postgres::TimePointTz updated_at;
};

struct NoteForCreate {
    UserId user_id;
    std::string name;
    std::string description;
};

struct NoteForUpdate {
    NoteId note_id;
    std::optional<std::string> name;
    std::optional<std::string> description;
};

struct Cursor {
    userver::storages::postgres::TimePointTz updated_at;
    NoteId id;
};

std::optional<Cursor> DeserializeCursorFromString(const std::optional<std::string>& cursor);

std::optional<std::string> SerializeCursorToString(const std::optional<Cursor>& cursor);

}  // namespace notes::contract::models

namespace userver::storages::postgres::io {

template <>
struct CppToUserPg<notes::contract::models::Status> : EnumMappingBase<notes::contract::models::Status> {
    static constexpr DBTypeName postgres_name = "notes.note_status";
    static constexpr EnumeratorList enumerators{{EnumType::kActive, "active"}, {EnumType::kDeleted, "deleted"}};
};

}  // namespace userver::storages::postgres::io
