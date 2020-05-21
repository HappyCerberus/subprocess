//
// Created by Happy on 21/05/2020.
//

#include "LinuxErrorHandler.h"

#include <errno.h>
#include <string.h>

namespace util {

// TODO: figure out a way how to switch to POSIX version of strerror_r that allows us to loop retry generating the
//  string.
std::string LinuxErrorHandler::GetError() {
    int err = errno;
    size_t len = 1024;
    char *buff = static_cast<char *>(malloc(len));
    char *res = strerror_r(err, buff, len);
    std::string result(res);
    free(buff);
    return result;
}

}// namespace util
