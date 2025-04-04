#include <iostream>
#include <queue>
#include <vector>
#include <csignal>
#include <sys/time.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "job.h"

#define QUANTUM 100
#define IO_TIME 40

using namespace std;

int remaining_time;
string algorithm;
struct job* running = nullptr;

vector<struct job*> completed_jobs;
vector<struct job*> blocked_jobs;

queue<struct job*> feedback_queues[4];
vector<struct job*> srt_ready;

int current_time = 0;

void print_job_stats(struct job* j, int completion_time) {
    int turnaround_time = completion_time - j->w;
    float norm_turnaround = (float)turnaround_time / (j->total_time);
    printf("Job %d:\n", j->id);
    printf(" Arrival time: %d\n", j->w);
    printf(" Completion time: %d\n", completion_time);
    printf(" Service time: %d\n", j->total_time);
    printf(" Turnaround time: %d\n", turnaround_time);
    printf(" Normalized Turnaround Time: %.2f\n", norm_turnaround);
}

void handle_job_completion(struct job* j) {
    print_job_stats(j, current_time);
    completed_jobs.push_back(j);
}

void try_add_new_job() {
    if (rand() % 100 < 2) { // 2% chance
        struct job* new_job = generate_next_job();
        new_job->w = current_time;

        if (algorithm == "srt") {
            srt_ready.push_back(new_job);
        } else {
            feedback_queues[0].push(new_job);
        }
    }
}

void check_blocked_jobs() {
    auto it = blocked_jobs.begin();
    while (it != blocked_jobs.end()) {
        struct job* j = *it;
        if (current_time - j->e >= IO_TIME) {
            j->next_interrupt++;
            if (algorithm == "srt") {
                srt_ready.push_back(j);
            } else {
                feedback_queues[3].push(j); // return to lowest queue
            }
            it = blocked_jobs.erase(it);
        } else {
            ++it;
        }
    }
}

struct job* select_next_job_srt() {
    if (srt_ready.empty()) return nullptr;
    auto shortest_it = srt_ready.begin();
    for (auto it = srt_ready.begin(); it != srt_ready.end(); ++it) {
        if ((*it)->time_remaining < (*shortest_it)->time_remaining) {
            shortest_it = it;
        }
    }
    struct job* next = *shortest_it;
    srt_ready.erase(shortest_it);
    return next;
}

struct job* select_next_job_feedback() {
    for (int i = 0; i < 4; ++i) {
        if (!feedback_queues[i].empty()) {
            struct job* j = feedback_queues[i].front();
            feedback_queues[i].pop();
            return j;
        }
    }
    return nullptr;
}

void dispatcher(int signum) {
    current_time++;

    if (running) {
        running->time_remaining--;

        if (running->next_interrupt < running->num_interrupts &&
            (running->total_time - running->time_remaining) >= running->interrupts[running->next_interrupt]) {
            // block job
            running->e = current_time;
            blocked_jobs.push_back(running);
            running = nullptr;
        } else if (running->time_remaining <= 0) {
            handle_job_completion(running);
            running = nullptr;
        } else {
            if (algorithm == "feedback") {
                // demote job to next lower queue
                int new_queue = 3; // default lowest
                feedback_queues[new_queue].push(running);
                running = nullptr;
            } else {
                srt_ready.push_back(running);
                running = nullptr;
            }
        }
    }

    try_add_new_job();
    check_blocked_jobs();

    if (!running) {
        if (algorithm == "srt") {
            running = select_next_job_srt();
        } else {
            running = select_next_job_feedback();
        }

        if (running && running->e == 0) {
            running->e = current_time;
        }
    }

    remaining_time--;
    if (remaining_time <= 0) {
        exit(0);
    }

    // Reset timer
    struct itimerval tm;
    memset(&tm, 0, sizeof(tm));
    tm.it_value.tv_sec = 0;
    tm.it_value.tv_usec = 1000 * 1; // 1ms
    setitimer(ITIMER_VIRTUAL, &tm, nullptr);
}

int main(int argc, char** argv) {
    if (argc != 4) {
        cerr << "Usage: ./sim <random_seed> <algorithm: srt|feedback> <duration_ms>\n";
        return 1;
    }

    srandom(atoi(argv[1]));
    algorithm = argv[2];
    remaining_time = atoi(argv[3]);

    if (algorithm != "srt" && algorithm != "feedback") {
        cerr << "Algorithm must be either 'srt' or 'feedback'\n";
        return 1;
    }

    signal(SIGVTALRM, dispatcher);

    // Preload one job
    struct job* first_job = generate_next_job();
    first_job->w = current_time;
    if (algorithm == "srt") {
        srt_ready.push_back(first_job);
    } else {
        feedback_queues[0].push(first_job);
    }

    // Start timer
    struct itimerval tm;
    memset(&tm, 0, sizeof(tm));
    tm.it_value.tv_sec = 0;
    tm.it_value.tv_usec = 1000 * 1; // 1ms
    setitimer(ITIMER_VIRTUAL, &tm, nullptr);

    while (true); // simulation loop

    return 0;
}
