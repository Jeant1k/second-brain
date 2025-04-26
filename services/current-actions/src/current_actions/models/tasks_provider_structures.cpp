#include "tasks_provider_structures.hpp"

#include <iomanip>

#include <boost/uuid/uuid_generators.hpp>

namespace current_actions::models {

namespace {

using handlers::TaskStatus;
using d_duration = std::chrono::duration<double>;

std::chrono::system_clock::time_point DeserializeStringToTimePoint(std::string_view timestamp_begin) {
    d_duration since_epoch_full{std::stod(std::string(timestamp_begin))};

    return std::chrono::system_clock::time_point{
        std::chrono::duration_cast<std::chrono::system_clock::duration>(since_epoch_full)
    };
}

std::string SerializeTimePointToString(std::chrono::system_clock::time_point tp) {
    std::stringstream ss;
    ss << std::setprecision(16) << std::chrono::duration_cast<d_duration>(tp.time_since_epoch()).count();
    return ss.str();
}

}  // namespace

std::optional<handlers::Priority> Transform(const std::optional<Priority> priority) {
    if (!priority.has_value()) {
        return std::nullopt;
    }

    switch (priority.value()) {
        case Priority::kHigh:
            return handlers::Priority::kHigh;
        case Priority::kLow:
            return handlers::Priority::kLow;
        case Priority::kMedium:
            return handlers::Priority::kMedium;
    }
}

std::optional<Priority> Transform(const std::optional<handlers::Priority> priority) {
    if (!priority.has_value()) {
        return std::nullopt;
    }

    switch (priority.value()) {
        case handlers::Priority::kHigh:
            return Priority::kHigh;
        case handlers::Priority::kLow:
            return Priority::kLow;
        case handlers::Priority::kMedium:
            return Priority::kMedium;
    }
}

std::optional<handlers::TaskStatus> Transform(const std::optional<Status> status) {
    if (!status.has_value()) {
        return std::nullopt;
    }

    switch (status.value()) {
        case Status::kActive:
            return TaskStatus::kActive;
        case Status::kCompleted:
            return TaskStatus::kCompleted;
    }
}

std::optional<Status> Transform(const std::optional<handlers::TaskStatus> status) {
    if (!status.has_value()) {
        return std::nullopt;
    }

    switch (status.value()) {
        case TaskStatus::kActive:
            return Status::kActive;
        case TaskStatus::kCompleted:
            return Status::kCompleted;
    }
}

std::optional<Cursor> DeserializeCursorFromString(const std::optional<std::string>& cursor) {
    if (!cursor.has_value()) {
        return std::nullopt;
    }

    const auto separator_position = cursor.value().find('_');
    UINVARIANT(
        separator_position != std::string::npos, "cursor is not valid: `_` separator not found" + cursor.value()
    );

    const auto cursor_id = boost::uuids::string_generator()(cursor.value().substr(0, separator_position));
    const auto cursor_updated_at = cursor.value().substr(separator_position + 1);

    return {
        {userver::storages::postgres::TimePointTz{DeserializeStringToTimePoint(cursor_updated_at)}, TaskId{cursor_id}}
    };
}

std::optional<std::string> SerializeCursorToString(const std::optional<Cursor>& cursor) {
    if (!cursor.has_value()) {
        return std::nullopt;
    }

    return fmt::format(
        "{}_{}",
        boost::uuids::to_string(cursor.value().id.GetUnderlying()),
        SerializeTimePointToString(cursor.value().updated_at.GetUnderlying())
    );
}

}  // namespace current_actions::models
