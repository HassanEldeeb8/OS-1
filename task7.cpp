#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

void readInput(string filename, int &numProcesses, int &numResources,
               vector<int> &E, vector<vector<int>> &C, vector<vector<int>> &R) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Unable to open file!" << endl;
        exit(1);
    }

    file >> numProcesses >> numResources;

    E.resize(numResources);
    for (int i = 0; i < numResources; i++)
        file >> E[i];

    C.assign(numProcesses, vector<int>(numResources));
    for (int i = 0; i < numProcesses; i++)
        for (int j = 0; j < numResources; j++)
            file >> C[i][j];

    R.assign(numProcesses, vector<int>(numResources));
    for (int i = 0; i < numProcesses; i++)
        for (int j = 0; j < numResources; j++)
            file >> R[i][j];

    file.close();
}

bool detectDeadlock(int numProcesses, int numResources, vector<int> &E,
                    vector<vector<int>> &C, vector<vector<int>> &R) {
    vector<int> A(numResources, 0);
    vector<bool> finished(numProcesses, false);
    vector<int> deadlockedProcesses;

    // Calculate Available Resources: A = E - sum(C[i])
    for (int j = 0; j < numResources; j++) {
        int sumAllocated = 0;
        for (int i = 0; i < numProcesses; i++)
            sumAllocated += C[i][j];
        A[j] = E[j] - sumAllocated;
    }

    // Deadlock Detection Algorithm
    while (true) {
        bool foundProcess = false;

        for (int i = 0; i < numProcesses; i++) {
            if (!finished[i]) {
                bool canExecute = true;

                for (int j = 0; j < numResources; j++) {
                    if (R[i][j] > A[j]) {
                        canExecute = false;
                        break;
                    }
                }

                if (canExecute) {
                    foundProcess = true;
                    finished[i] = true;
                    for (int j = 0; j < numResources; j++)
                        A[j] += C[i][j]; // Release resources
                }
            }
        }

        if (!foundProcess) break;
    }

    // Check for deadlock
    for (int i = 0; i < numProcesses; i++) {
        if (!finished[i]) {
            deadlockedProcesses.push_back(i);
        }
    }

    if (!deadlockedProcesses.empty()) {
        cout << "Deadlock detected! Processes in deadlock: ";
        for (int p : deadlockedProcesses)
            cout << "P" << p << " ";
        cout << endl;
        return true;
    } else {
        cout << "No deadlock detected." << endl;
        return false;
    }
}

int main() {
    int numProcesses, numResources;
    vector<int> E;
    vector<vector<int>> C, R;

    readInput("input.txt", numProcesses, numResources, E, C, R);
    detectDeadlock(numProcesses, numResources, E, C, R);

    return 0;
}
