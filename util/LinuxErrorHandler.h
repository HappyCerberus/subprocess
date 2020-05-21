//
// Created by Happy on 21/05/2020.
//

#ifndef SUBPROCESS_LINUXERRORHANDLER_H
#define SUBPROCESS_LINUXERRORHANDLER_H

#include <string>

namespace util {

class LinuxErrorHandler {
public:
    static std::string GetError();
};

}// namespace util

#endif//SUBPROCESS_LINUXERRORHANDLER_H
