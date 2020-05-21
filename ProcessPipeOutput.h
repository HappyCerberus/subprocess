//
// Created by Happy on 21/05/2020.
//

#ifndef SUBPROCESS_PROCESSPIPEOUTPUT_H
#define SUBPROCESS_PROCESSPIPEOUTPUT_H

#include "util/StatusOr.h"
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <ostream>

namespace io = boost::iostreams;

class ProcessPipeOutput {
public:
    ProcessPipeOutput();
    util::StatusOr<std::pair<int, int>> GetFDs();
    io::stream<io::file_descriptor_source> &Stream() { return source_; }

private:
    io::file_descriptor_source fd_;
    io::stream<io::file_descriptor_source> source_;
};


#endif//SUBPROCESS_PROCESSPIPEOUTPUT_H
