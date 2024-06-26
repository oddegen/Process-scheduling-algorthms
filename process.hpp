#ifndef PROCESS_H
#define PROCESS_H

typedef int pid_t;

class Process
{
public:
    pid_t _pid;
    int _arrival_time;
    int _burst_time;
    int _remaining_time;
    int _start_time;
    int _completion_time;
    int _waiting_time;
    int _turnaround_time;
    int _response_time;

    Process(pid_t pid, int arrival, int burst)
        : _pid(pid), _arrival_time(arrival), _burst_time(burst),
          _remaining_time(burst), _start_time(-1), _completion_time(-1),
          _waiting_time(-1), _turnaround_time(-1) {}
};

#endif