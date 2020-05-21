//
// Created by Happy on 21/05/2020.
//

#include "ProcessOutput.h"
#include "util/LinuxErrorHandler.h"

#include <fcntl.h>

ProcessOutput::ProcessOutput(std::string filepath, FileOpenAction action, int access_rights)
    : filepath_(std::move(filepath)), action_(action), access_rights_(access_rights), fd_(-1) {}

util::StatusOr<int> ProcessOutput::GetFD() const {
    if (fd_ >= 0) return fd_;

    int flags = O_CREAT | O_WRONLY;
    if (action_ == MODE_APPEND) flags |= O_APPEND;
    if (action_ == MODE_TRUNCATE) flags |= O_TRUNC;
    if (action_ == MODE_CREATE_EXCLUSIVE) flags |= O_EXCL;

    int ret = open(filepath_.c_str(), flags, access_rights_);
    if (ret < 0) { absl::Status(absl::StatusCode::kInternal, util::LinuxErrorHandler::GetError()); }
    return ret;
}
ProcessOutput::ProcessOutput(int fd) : fd_(fd) {}
