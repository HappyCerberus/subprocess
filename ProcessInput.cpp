//
// Created by Happy on 21/05/2020.
//

#include "ProcessInput.h"
#include "util/LinuxErrorHandler.h"

#include <fcntl.h>

ProcessInput::ProcessInput(std::string filepath) : filepath_(std::move(filepath)), fd_(-1) {}
util::StatusOr<int> ProcessInput::GetFD() const {
    if (fd_ >= 0) return fd_;

    int ret = open(filepath_.c_str(), O_RDONLY);
    if (ret < 0) { absl::Status(absl::StatusCode::kInternal, util::LinuxErrorHandler::GetError()); }
    return ret;
}
ProcessInput::ProcessInput(int fd) : fd_(fd) {}
