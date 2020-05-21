//
// Created by Happy on 21/05/2020.
//

#include "ProcessPipeInput.h"
#include "util/LinuxErrorHandler.h"

#include <absl/strings/str_cat.h>
#include <unistd.h>

ProcessPipeInput::ProcessPipeInput() : sink_{} {}

util::StatusOr<std::pair<int, int>> ProcessPipeInput::GetFDs() {
    int fd[2];
    int ret = pipe(fd);
    if (ret == -1) {
        return absl::InternalError(absl::StrCat("Failed to create input pipe : ", util::LinuxErrorHandler::GetError()));
    }

    fd_.open(fd[1], io::close_handle);
    sink_.open(fd_);
    return std::make_pair(fd[0], fd[1]);
}
