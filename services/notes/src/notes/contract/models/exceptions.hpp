#include <exception>
#include <string>

namespace notes::contract::models {

class NoteNotFoundException : public std::exception {
private:
    const std::string error_message_;

public:
    explicit NoteNotFoundException(std::string&& message) : error_message_(std::move(message)) {}

    const char* what() const noexcept override { return error_message_.c_str(); }
};

class NoFieldsProvidedException : public std::exception {
private:
    const std::string error_message_;

public:
    explicit NoFieldsProvidedException(std::string&& message) : error_message_(std::move(message)) {}

    const char* what() const noexcept override { return error_message_.c_str(); }
};

class WrongUserIdException : public std::exception {
private:
    const std::string error_message_;

public:
    explicit WrongUserIdException(std::string&& message) : error_message_(std::move(message)) {}

    const char* what() const noexcept override { return error_message_.c_str(); }
};

}  // namespace notes::contract::models