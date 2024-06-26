#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <atomic>
#include <iostream>
#include <csignal>
#include <chrono>
#include <thread>

/*
This program demonstrates strict alternation between two processes using
shared memory for synchronization. Two child processes are created, each
running the `process_function`. The `turn` variable, stored in shared memory,
ensures that the processes take turns to print their respective messages to
the standard output.
*/

void process_function(int process_number, std::atomic<int> *turn)
{
    while (true)
    {
        while (turn->load() != process_number)
            ;
        std::cout << "Process " << process_number << " turn." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        turn->store(1 - process_number);
    }
}

int main()
{
    std::atomic<int> *turn = static_cast<std::atomic<int> *>(mmap(nullptr, sizeof(std::atomic<int>),
                                                                  PROT_READ | PROT_WRITE,
                                                                  MAP_SHARED | MAP_ANONYMOUS,
                                                                  -1, 0));
    if (turn == MAP_FAILED)
    {
        perror("mmap");
        return EXIT_FAILURE;
    }
    new (turn) std::atomic<int>(0);

    pid_t pid1 = fork();
    if (pid1 == 0)
    {
        process_function(0, turn);
    }

    pid_t pid2 = fork();
    if (pid2 == 0)
    {
        process_function(1, turn);
    }

    // Parent process waits for child processes to finish.
    wait(nullptr);

    // Clean up the shared memory.
    munmap(turn, sizeof(std::atomic<int>));

    return 0;
}
