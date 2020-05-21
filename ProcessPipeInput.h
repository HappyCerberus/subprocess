//
// Created by Happy on 21/05/2020.
//

#ifndef SUBPROCESS_PROCESSPIPEINPUT_H
#define SUBPROCESS_PROCESSPIPEINPUT_H

#include "util/StatusOr.h"
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <ostream>

namespace io = boost::iostreams;

class ProcessPipeInput {
public:
    ProcessPipeInput();
    util::StatusOr<std::pair<int, int>> GetFDs();
    io::stream<io::file_descriptor_sink> &Stream() { return sink_; }

private:
    io::file_descriptor_sink fd_;
    io::stream<io::file_descriptor_sink> sink_;
};


#endif//SUBPROCESS_PROCESSPIPEINPUT_H
