# Subprocess
C++ wrapper around Linux subprocess creation and control.

The goal of this library is to provide the following functionality:

- thread safety
- subprocess manipulation
  - interacting with STDIN, STDOUT, STDERR
  - terminating subprocesses
  - flags & environment manipulation on startup
- zombie process prevention
  - including after main process crash
- deadline support
- controlling resource limits
