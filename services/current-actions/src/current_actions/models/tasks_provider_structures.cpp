#include "tasks_provider_structures.hpp"

#include <iomanip>

#include <boost/uuid/uuid_generators.hpp>

namespace current_actions::models {

namespace {

using d_duration = std::chrono::duration<double>;

std::chrono::system_clock::time_point DeserializeStringToTimePoint(std::string_view timestamp_begin) {
    d_duration since_epoch_full{std::stod(std::string(timestamp_begin))};

    return std::chrono::system_clock::time_point{
        std::chrono::duration_cast<std::chrono::system_clock::duration>(since_epoch_full)};
}

std::string SerializeTimePointToString(std::chrono::system_clock::time_point tp) {
    std::stringstream ss;
    ss << std::setprecision(16) << std::chrono::duration_cast<d_duration>(tp.time_since_epoch()).count();
    return ss.str();
}

}  // namespace

Cursor DeserializeCursorFromString(const std::string& cursor) {
    const auto separator_position = cursor.find('_');
    UINVARIANT(separator_position != std::string::npos, "cursor is not valid: `_` separator not found" + cursor);

    const auto cursor_id = boost::uuids::string_generator()(cursor.substr(0, separator_position));
    const auto cursor_updated_at = cursor.substr(separator_position + 1);

    return {userver::storages::postgres::TimePointTz{DeserializeStringToTimePoint(cursor_updated_at)}, TaskId{cursor_id}};
}

std::string SerializeCursorToString(const Cursor& cursor) {
    return fmt::format(
        "{}_{}", cursor.id, SerializeTimePointToString(cursor.updated_at.GetUnderlying())
    );
}

}  // namespace current_actions::models
