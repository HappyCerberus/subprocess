#include <iostream>

/*
 * Open Questions:
 * - error handling
 *   - abort() when the library itself fails (creates core dumps)
 *   - return a user specified number, that is outside of the range of errors reported by the executable
 * - integration with ostream & istream
 *   - boost::iostreams::file_descriptor seems broken
 * - thread safety
 *   - thread exclusion on top of a single instance
 * - subprocess manipulation
 *   - Start(), Kill()
 * - zombie process prevention after main process crash
 * - deadline support
 * - controlling resource limits
 */

#include "SubProcess.h"

#include "ProcessPipeInput.h"
#include "ProcessPipeOutput.h"

namespace io = boost::iostreams;

int main(int argc, char *argv[], char *env[]) {

    //ProcessOutput out("/home/simon/out.txt", MODE_TRUNCATE, S_IRUSR | S_IWUSR);
    /*
    ProcessInput in("/home/simon/simple.txt");
    ProcessOutput out(STDOUT_FILENO);
    ProcessOutput err("/dev/null", MODE_APPEND, 0);
    SubProcess process("/bin/ls", RETRY_NEVER, std::vector<std::string>{"-la", "/home/simon/simple.txt"});
    */

    ProcessPipeInput in;
    ProcessPipeOutput out;
    ProcessPipeOutput err;
    SubProcess process("/usr/bin/bc", RETRY_NEVER);

    auto s = process.Run([&in] { return in.GetFDs(); }, [&out] { return out.GetFDs(); },
                         [&err] { return err.GetFDs(); });
    if (!s.ok()) { std::cerr << "Fail to run process : " << s.status().message() << std::endl; }
    std::cout << "Process finished running with return code " << s.ValueOrDie() << std::endl;
    return 0;
}
