#include <exception>
#include <string>

namespace current_actions::contract::models {

class TaskNotFoundException : public std::exception {
private:
    const std::string error_message_;

public:
    explicit TaskNotFoundException(std::string&& message) : error_message_(std::move(message)) {}

    const char* what() const noexcept override { return error_message_.c_str(); }
};

class NoFieldsProvidedException : public std::exception {
    private:
        const std::string error_message_;
    
    public:
        explicit NoFieldsProvidedException(std::string&& message) : error_message_(std::move(message)) {}
    
        const char* what() const noexcept override { return error_message_.c_str(); }
    };

}  // namespace current_actions::contract::models