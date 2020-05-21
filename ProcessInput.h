//
// Created by Happy on 21/05/2020.
//

#ifndef SUBPROCESS_PROCESSINPUT_H
#define SUBPROCESS_PROCESSINPUT_H


#include "util/StatusOr.h"
#include <string>
class ProcessInput {
public:
    ProcessInput(std::string filepath);
    ProcessInput(int fd);
    util::StatusOr<int> GetFD() const;

private:
    std::string filepath_;
    int fd_;
};


#endif//SUBPROCESS_PROCESSINPUT_H
