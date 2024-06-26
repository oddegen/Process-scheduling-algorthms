#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <mutex>
#include <iomanip>

#include "process.hpp"

/*
 * Shortest Remaining Time (SRT) Scheduling Algorithm
 *
 * The Shortest Remaining Time (SRT) scheduling algorithm is a preemptive scheduling 
 * algorithm in which the process with the smallest amount of remaining time until completion 
 * is selected to execute. If a new process arrives with a burst time smaller than 
 * the remaining time of the current process, the current process is preempted and the 
 * new process is scheduled to run. This ensures that the CPU is always executing the 
 * process with the shortest remaining time.
 *
 * Steps of the SRT algorithm:
 * 1. At any given time, the process with the shortest remaining burst time is selected.
 * 2. If a new process arrives with a shorter burst time than the current process's 
 *    remaining time, preempt the current process and schedule the new process.
 * 3. Continue this process until all processes are completed.
 * 4. Calculate the completion time, turnaround time, and waiting time for each process.
 *
 * This implementation uses a priority queue to manage processes based on their 
 * remaining burst time and a vector to store process information. A mutex is used 
 * to synchronize access to the priority queue.
 */

class SRTScheduler
{
public:
    SRTScheduler(std::vector<Process> &processes) : _processes(processes) {}

    void schedule()
    {
        std::sort(_processes.begin(), _processes.end(), [](const Process &p1, const Process &p2)
                  { return p1._arrival_time < p2._arrival_time; });

        auto cmp = [](Process const &p1, Process const &p2)
        {
            return p1._remaining_time > p2._remaining_time;
        };
        std::priority_queue<Process, std::vector<Process>, decltype(cmp)> pq(cmp);

        int current_time = 0;
        int completed = 0;
        int n = _processes.size();

        while (completed != n)
        {
            {
                std::lock_guard<std::mutex> lock(_pq_mutex);

                for (auto &process : _processes)
                {
                    if (process._arrival_time == current_time)
                    {
                        pq.push(process);
                    }
                }
            }

            if (!pq.empty())
            {
                Process current_process = pq.top();
                pq.pop();

                if (current_process._start_time == -1)
                {
                    current_process._start_time = current_time;
                }

                usleep(1000);
                current_time++;
                current_process._remaining_time--;

                if (current_process._remaining_time == 0)
                {
                    current_process._completion_time = current_time;
                    current_process._turnaround_time = current_process._completion_time - current_process._arrival_time;
                    current_process._waiting_time = current_process._turnaround_time - current_process._burst_time;
                    _processes[current_process._pid] = current_process;
                    completed++;
                }
                else
                {
                    pq.push(current_process);
                }
            }
            else
            {
                usleep(1000);
                current_time++;
            }
        }
    }

    void printResults()
    {
        std::cout << std::left << std::setw(5) << "PID"
                  << std::setw(10) << "Arrival"
                  << std::setw(10) << "Burst"
                  << std::setw(10) << "Start"
                  << std::setw(15) << "Completion"
                  << std::setw(15) << "Turnaround"
                  << std::setw(10) << "Waiting"
                  << "\n";

        for (const auto &process : _processes)
        {
            std::cout << std::left << std::setw(5) << process._pid
                      << std::setw(10) << process._arrival_time
                      << std::setw(10) << process._burst_time
                      << std::setw(10) << process._start_time
                      << std::setw(15) << process._completion_time
                      << std::setw(15) << process._turnaround_time
                      << std::setw(10) << process._waiting_time
                      << "\n";
        }
    }

private:
    std::vector<Process> &_processes;
    std::mutex _pq_mutex;
};

void *process_function(void *arg)
{
    Process *process = static_cast<Process *>(arg);
    usleep(process->_arrival_time * 1000);
    return nullptr;
}

int main()
{
    std::vector<Process> processes = {
        {0, 0, 8}, {1, 1, 4}, {2, 2, 9}, {3, 3, 5}};

    SRTScheduler scheduler(processes);
    scheduler.schedule();
    scheduler.printResults();

    return EXIT_SUCCESS;
}
