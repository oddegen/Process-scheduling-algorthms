#include <iostream>
#include <vector>
#include <queue>
#include <iomanip>

#include "process.hpp"

/*
 * Round Robin (RR) Scheduling Algorithm
 *
 * The Round Robin (RR) scheduling algorithm is a preemptive scheduling algorithm
 * where each process is assigned a fixed time slice (quantum) and the CPU scheduler
 * switches between processes once their time slice expires. If a process has not
 * completed execution, it is placed at the end of the ready queue and the next
 * process in the queue is selected to run.
 *
 * Steps of the RR algorithm:
 * 1. Initialize a ready queue to hold processes.
 * 2. Assign a fixed time slice (quantum) for each process.
 * 3. Repeat until all processes are completed:
 *    a. Select the first process in the ready queue.
 *    b. Run the process for the quantum or until it completes.
 *    c. If the process completes within the quantum, calculate turnaround time. 
 *       Remove the process from the queue.
 *    d. If the process does not complete within the quantum, move it to the end
 *       of the queue.
 * 4. Calculate the average turnaround time and average response time for all processes.
 *
 */

class RRScheduler
{
public:
    RRScheduler(std::vector<Process> &processes, int quantum) : _processes(processes), _quantum(quantum) {}

    void schedule()
    {
        std::queue<int> ready_queue;
        int current_time = 0;
        int completed = 0;
        int n = _processes.size();
        std::vector<int> completed_process(n, 0);

        for (int i = 0; i < n; i++)
        {
            _processes[i]._remaining_time = _processes[i]._burst_time;
        }

        ready_queue.push(0);

        while (completed < n)
        {
            if (!ready_queue.empty())
            {
                int current_process_pid = ready_queue.front();
                ready_queue.pop();

                if (_processes[current_process_pid]._remaining_time == _processes[current_process_pid]._burst_time)
                {
                    _processes[current_process_pid]._response_time = current_time - _processes[current_process_pid]._arrival_time;
                }

                int time_slice = std::min(_quantum, _processes[current_process_pid]._remaining_time);
                _processes[current_process_pid]._remaining_time -= time_slice;
                current_time += time_slice;

                if (_processes[current_process_pid]._remaining_time == 0)
                {
                    completed_process[current_process_pid] = 1;
                    completed++;
                    _processes[current_process_pid]._turnaround_time = current_time - _processes[current_process_pid]._arrival_time;
                }

                for (int i = 0; i < n; i++)
                {
                    if (!completed_process[i] && _processes[i]._arrival_time <= current_time && _processes[i]._remaining_time > 0)
                    {
                        bool already_in_queue = false;
                        std::queue<int> temp_queue = ready_queue;
                        while (!temp_queue.empty())
                        {
                            if (temp_queue.front() == i)
                            {
                                already_in_queue = true;
                                break;
                            }
                            temp_queue.pop();
                        }
                        if (!already_in_queue)
                        {
                            ready_queue.push(i);
                        }
                    }
                }

                if (!completed_process[current_process_pid])
                {
                    ready_queue.push(current_process_pid);
                }

                if (ready_queue.empty() && completed < n)
                {
                    for (int i = 0; i < n; i++)
                    {
                        if (!completed_process[i])
                        {
                            ready_queue.push(i);
                            current_time = _processes[i]._arrival_time;
                            break;
                        }
                    }
                }
            }
        }
    }

    void printResults()
    {
        std::cout << std::left << std::setw(5) << "PID"
                  << std::setw(10) << "Arrival"
                  << std::setw(10) << "Burst"
                  << std::setw(10) << "Response"
                  << std::setw(10) << "Turnaround"
                  << "\n";

        for (const auto &process : _processes)
        {
            std::cout << std::left << std::setw(5) << process._pid
                      << std::setw(10) << process._arrival_time
                      << std::setw(10) << process._burst_time
                      << std::setw(10) << process._response_time
                      << std::setw(10) << process._turnaround_time
                      << "\n";

            _total_turnaround_time += process._turnaround_time;
            _total_response_time += process._response_time;
        }

        double avg_turnaround_time = static_cast<double>(_total_turnaround_time) / _processes.size();
        double avg_response_time = static_cast<double>(_total_response_time) / _processes.size();

        std::cout << "\nAverage Turnaround Time: " << avg_turnaround_time << " ms\n";
        std::cout << "Average Response Time: " << avg_response_time << " ms\n";
    }

private:
    std::vector<Process> &_processes;
    int _quantum;
    int _total_turnaround_time = 0;
    int _total_response_time = 0;
};

int main()
{
    std::vector<Process> processes = {
        {0, 0, 8}, {1, 1, 4}, {2, 2, 9}, {3, 3, 5}};
    int quantum = 3;

    RRScheduler scheduler(processes, quantum);
    scheduler.schedule();
    scheduler.printResults();

    return EXIT_SUCCESS;
}
