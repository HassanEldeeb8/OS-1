#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unordered_map>
#include <queue>

using namespace std;

// Function to generate a page reference string
vector<int> generatePageReferenceString(int N, int length, double p) {
    vector<int> referenceString;
    int currentPage = rand() % N; // Start with a random page
    
    for (int i = 0; i < length; i++) {
        referenceString.push_back(currentPage);
        if (((double)rand() / RAND_MAX) > p) { // Transition to a different page
            currentPage = rand() % N;
        }
    }
    return referenceString;
}

// FIFO Page Replacement Policy
int fifoPageReplacement(const vector<int>& referenceString, int numFrames) {
    unordered_map<int, bool> pageTable;
    queue<int> pageQueue;
    int pageFaults = 0;

    for (int page : referenceString) {
        if (pageTable.find(page) == pageTable.end()) { // Page fault
            if (pageQueue.size() >= numFrames) {
                int victim = pageQueue.front();
                pageQueue.pop();
                pageTable.erase(victim);
            }
            pageQueue.push(page);
            pageTable[page] = true;
            pageFaults++;
        }
    }
    return pageFaults;
}

// Simulate local and global page replacement for two processes
void simulatePageReplacement(int N, int length, double p, int numFramesPerProcess) {
    vector<int> process1 = generatePageReferenceString(N, length, p);
    vector<int> process2 = generatePageReferenceString(N, length, p);
    
    cout << "Process 1 Page Reference String: ";
    for (int page : process1) cout << page << " ";
    cout << endl;
    
    cout << "Process 2 Page Reference String: ";
    for (int page : process2) cout << page << " ";
    cout << endl;

    // Local Replacement: Each process gets a fixed number of frames
    int localFaults1 = fifoPageReplacement(process1, numFramesPerProcess);
    int localFaults2 = fifoPageReplacement(process2, numFramesPerProcess);
    cout << "Local Replacement - Process 1 Faults: " << localFaults1 << endl;
    cout << "Local Replacement - Process 2 Faults: " << localFaults2 << endl;

    // Global Replacement: Both processes share frames dynamically
    vector<int> combinedReference;
    combinedReference.insert(combinedReference.end(), process1.begin(), process1.end());
    combinedReference.insert(combinedReference.end(), process2.begin(), process2.end());
    int globalFaults = fifoPageReplacement(combinedReference, numFramesPerProcess * 2);
    cout << "Global Replacement - Total Faults: " << globalFaults << endl;
}

int main() {
    srand(time(0));
    int N = 5; // Number of unique pages
    int length = 20; // Length of page reference string
    double p = 0.7; // Probability of staying on the same page
    int numFramesPerProcess = 3; // Frames allocated to each process in local replacement

    simulatePageReplacement(N, length, p, numFramesPerProcess);
    return 0;
}
