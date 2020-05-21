//
// Created by Happy on 21/05/2020.
//

#ifndef SUBPROCESS_PROCESSOUTPUT_H
#define SUBPROCESS_PROCESSOUTPUT_H


#include "util/StatusOr.h"
#include <string>
enum FileOpenAction {
    MODE_APPEND,
    MODE_TRUNCATE,
    MODE_CREATE_EXCLUSIVE,
};

class ProcessOutput {
public:
    ProcessOutput(std::string filepath, FileOpenAction action, int access_rights);
    ProcessOutput(int fd);
    util::StatusOr<int> GetFD() const;

private:
    std::string filepath_;
    FileOpenAction action_;
    int access_rights_;
    int fd_;
};


#endif//SUBPROCESS_PROCESSOUTPUT_H
