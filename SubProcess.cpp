//
// Created by Happy on 21/05/2020.
//

#include "SubProcess.h"
#include "util/LinuxErrorHandler.h"

#include <absl/strings/str_cat.h>
#include <absl/strings/str_format.h>
#include <errno.h>
#include <filesystem>
#include <iostream>
#include <memory.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <system_error>
#include <unistd.h>
#include <vector>

SubProcess::SubProcess(std::string executable, RetryOnResourceExhaustion retry_policy, std::vector<std::string> argv,
                       std::vector<std::string> env)
    : retry_policy_(retry_policy), executable_(std::move(executable)), argv_(std::move(argv)), env_(std::move(env)) {}

util::StatusOr<int> SubProcess::Run() { return Run(STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO); }

util::StatusOr<int> SubProcess::Run(std::pair<int, int> in, std::pair<int, int> out, std::pair<int, int> err) {
    pid_t pid_;
    do {
        pid_ = fork();
        // TODO add backoff policy
    } while (pid_ < 0 && errno == EAGAIN && retry_policy_ != RETRY_NEVER);

    if (pid_ < 0) {
        return absl::InternalError(absl::StrCat("Failed to fork new process : ", util::LinuxErrorHandler::GetError()));
    }

    // O_TMPFILE mode

    if (pid_ == 0) {// child
        SetupChild(in.first, out.second, err.second);
    }

    int ret;
    while ((ret = close(in.first)) == -1 && errno == EINTR)
        ;
    if (ret == -1) {
        return absl::InternalError(absl::StrFormat("Failed to close STDIN file descriptor from fd{%d}", out.first));
    }
    while ((ret = close(out.second)) == -1 && errno == EINTR)
        ;
    if (ret == -1) {
        return absl::InternalError(absl::StrFormat("Failed to close STDOUT file descriptor from fd{%d}", out.second));
    }
    while ((ret = close(err.second)) == -1 && errno == EINTR)
        ;
    if (ret == -1) {
        return absl::InternalError(absl::StrFormat("Failed to close STDERR file descriptor from fd{%d}", err.second));
    }

    return Join();
}

util::StatusOr<int> SubProcess::Run(int in, int out, int err) {
    pid_t pid_;
    do {
        pid_ = fork();
        // TODO add backoff policy
    } while (pid_ < 0 && errno == EAGAIN && retry_policy_ != RETRY_NEVER);

    if (pid_ < 0) {
        return absl::InternalError(absl::StrCat("Failed to fork new process : ", util::LinuxErrorHandler::GetError()));
    }

    // O_TMPFILE mode

    if (pid_ == 0) {// child
        SetupChild(in, out, err);
    }

    return Join();
}

absl::Status SubProcess::ClosePids(std::vector<int> whitelist) {
    int mypid = getpid();

    std::stringstream s;
    s << "/proc/" << mypid << "/fd";

    std::filesystem::path path(s.str());
    std::error_code err;
    if (!std::filesystem::exists(path, err)) {
        return absl::InternalError(absl::StrFormat("Unable to find file descriptor directory {%s}.", s.str()));
    }
    if (!std::filesystem::is_directory(path, err)) {
        return absl::InternalError(absl::StrFormat("File descriptor directory is not a directory {%s}.", s.str()));
    }
    // this can throw, but shouldn't (at least not for non-existent directory)
    std::vector<int> files;
    for (auto &p : std::filesystem::directory_iterator(s.str())) {
        int fd = std::stoi(p.path().stem());
        if (fd > STDERR_FILENO) files.push_back(fd);
    }

    for (int fd : files) {
        bool skip = false;
        for (int w : whitelist) {
            if (w == fd) {
                skip = true;
                break;
            }
        }
        if (skip) continue;
        int ret = close(fd);
        // TODO: investigate EBADF further.
        if (ret == -1 && errno != EBADF) {
            return absl::InternalError(absl::StrFormat("Failed to close file descriptor %d : %s.", fd,
                                                       util::LinuxErrorHandler::GetError()));
        }
    }

    return absl::OkStatus();
}

util::StatusOr<int> SubProcess::Join() {
    int wstatus;
    pid_t w;
    do {
        w = waitpid(pid_, &wstatus, WUNTRACED | WCONTINUED);
        if (w == -1) {
            if (errno != EINTR) { continue; }
            return absl::Status(absl::StatusCode::kInternal, absl::StrCat("Failed to waitpid on child process : ",
                                                                          util::LinuxErrorHandler::GetError()));
        }

        if (WIFEXITED(wstatus)) {
            return WEXITSTATUS(wstatus);
        } else if (WIFSIGNALED(wstatus)) {
            return absl::Status(absl::StatusCode::kAborted, absl::StrFormat("Child process was killed with signal %d"
                                                                            ".",
                                                                            WTERMSIG(wstatus)));
        }
    } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));

    // Unreachable.
    return 0;
}
char **SubProcess::GenerateEnv() {
    char **env = static_cast<char **>(malloc(sizeof(char *) * env_.size() + 1));
    for (size_t i = 0; i < argv_.size(); i++) {
        environ[i] = strdup(argv_[i].c_str());
        if (environ[i] == nullptr) abort();
    }
    environ[env_.size()] = nullptr;
    return env;
}

char **SubProcess::GenerateArgs() {
    char **args = static_cast<char **>(malloc(sizeof(char *) * argv_.size() + 2));
    args[0] = strdup(executable_.c_str());
    for (size_t i = 1; i <= argv_.size(); i++) {
        args[i] = strdup(argv_[i - 1].c_str());
        if (environ[i] == nullptr) abort();
    }
    args[argv_.size() + 1] = nullptr;
    return args;
}

void SubProcess::SetupChild(int in, int out, int err) {
    auto s = ClosePids(std::vector<int>{in, out, err});
    if (!s.ok()) {
        std::cerr << "Failed to close pids : " << s.message() << std::endl;
        abort();
    }

    int ret;
    while ((ret = dup2(in, STDIN_FILENO)) == -1 && errno == EINTR)
        ;
    if (ret == -1) {
        std::cerr << "Failed to duplicate STDIN file descriptor from fd{" << in << "}" << std::endl;
        abort();
    }
    while ((ret = dup2(out, STDOUT_FILENO)) == -1 && errno == EINTR)
        ;
    if (ret == -1) {
        std::cerr << "Failed to duplicate STDOUT file descriptor from fd{" << out << "}" << std::endl;
        abort();
    }
    while ((ret = dup2(err, STDERR_FILENO)) == -1 && errno == EINTR)
        ;
    if (ret == -1) {
        std::cerr << "Failed to duplicate STDERR file descriptor from fd{" << err << "}" << std::endl;
        abort();
    }

    s = ClosePids(std::vector<int>{});
    if (!s.ok()) {
        std::cerr << "Failed to close pids : " << s.message() << std::endl;
        abort();
    }

    execve(executable_.c_str(), GenerateArgs(), GenerateEnv());
    abort();
}
