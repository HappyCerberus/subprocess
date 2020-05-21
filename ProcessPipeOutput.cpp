//
// Created by Happy on 21/05/2020.
//

#include "ProcessPipeOutput.h"
#include "util/LinuxErrorHandler.h"
#include <absl/strings/str_cat.h>

ProcessPipeOutput::ProcessPipeOutput() : source_{} {}

util::StatusOr<std::pair<int, int>> ProcessPipeOutput::GetFDs() {
    int fd[2];
    int ret = pipe(fd);
    if (ret == -1) {
        return absl::InternalError(absl::StrCat("Failed to create input pipe : ", util::LinuxErrorHandler::GetError()));
    }

    fd_.open(fd[0], io::close_handle);
    source_.open(fd_);
    return std::make_pair(fd[0], fd[1]);
}