//
// Created by Happy on 21/05/2020.
//

#ifndef SUBPROCESS_SUBPROCESS_H
#define SUBPROCESS_SUBPROCESS_H

#include "ProcessInput.h"
#include "ProcessOutput.h"
#include <absl/strings/str_cat.h>
#include <string>
#include <sys/types.h>
#include <vector>

// Retry starting the process
enum RetryOnResourceExhaustion {
    RETRY_NEVER,
    RETRY_IMMEDIATE,
    RETRY_BACKOFF,
};

typedef std::function<util::StatusOr<int>()> GetFD;
typedef std::function<util::StatusOr<std::pair<int, int>>()> GetFDs;

class SubProcess {
public:
    SubProcess(std::string executable, RetryOnResourceExhaustion retry_policy,
               std::vector<std::string> argv = std::vector<std::string>{},
               std::vector<std::string> env = std::vector<std::string>{});

    int Start(const ProcessInput &in, const ProcessOutput &out, const ProcessOutput &err);
    int Kill(int signal);
    int KillAndWait(int signal /*, duration timeout */);

    /*! Run a child process using STDIN, STDOUT, STDERR of the parent.
     *
     * @return return value of the child on success, absl::Status on a failure, or if child was killed by a signal.
     */
    util::StatusOr<int> Run();

    /*! Run a child process, remapping its STDIN, STDOUT, STDERR to the given ProcessInput & ProcessOutput classes.
     *
     * @param in ProcessInput representing stdin.
     * @param out ProcessOutput representing stdout.
     * @param err ProcessOutput representing stderr.
     * @return return value of the child on success, absl::Status on a failure, or if child was killed by a signal.
     */
    template<typename A, typename B, typename C>
    util::StatusOr<int> Run(A in, B out, C err);

    /*! Block until the child process ends.
     *
     * @return return value of the child on success, absl::Status on waitpid failure, or if child was killed by a
     * signal.
     */
    util::StatusOr<int> Join();

private:
    util::StatusOr<int> Run(int in, int out, int err);
    util::StatusOr<int> Run(std::pair<int, int> in, std::pair<int, int> out, std::pair<int, int> err);
    absl::Status ClosePids(std::vector<int> whitelist);
    void SetupChild(int in, int out, int err);

    char **GenerateEnv();
    char **GenerateArgs();

    pid_t pid_;
    RetryOnResourceExhaustion retry_policy_;
    std::string executable_;
    std::vector<std::string> argv_;
    std::vector<std::string> env_;
};

template<typename A, typename B, typename C>
util::StatusOr<int> SubProcess::Run(A in, B out, C err) {
    auto i = in();
    if (!i.ok()) {
        return absl::InternalError(absl::StrCat("Failed to obtain file descriptor representing STDIN"
                                                " : ",
                                                i.status().message()));
    }
    auto o = out();
    if (!o.ok()) {
        return absl::InternalError(absl::StrCat("Failed to obtain file descriptor representing STDOUT"
                                                " : ",
                                                o.status().message()));
    }
    auto e = err();
    if (!e.ok()) {
        return absl::InternalError(absl::StrCat("Failed to obtain file descriptor representing STDERR"
                                                " : ",
                                                e.status().message()));
    }

    return Run(i.ValueOrDie(), o.ValueOrDie(), e.ValueOrDie());
}


#endif//SUBPROCESS_SUBPROCESS_H
