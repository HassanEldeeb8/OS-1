#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>

using namespace std;

mutex mtx;  // Mutex for thread-safe access to shared resources

// Function to clean and normalize words
string cleanWord(const string& word) {
    string cleaned;
    for (char c : word) {
        if (isalpha(c) || c == '\'') {
            cleaned += tolower(c);
        }
    }
    return cleaned;
}

// Function to count word frequency in a segment
void countWords(const vector<string>& segment, unordered_map<string, int>& localFreq) {
    for (const string& word : segment) {
        string cleanedWord = cleanWord(word);
        if (!cleanedWord.empty()) {
            localFreq[cleanedWord]++;
        }
    }
}

// Function executed by each thread
void threadFunction(const vector<string>& segment, unordered_map<string, int>& globalFreq) {
    unordered_map<string, int> localFreq;
    countWords(segment, localFreq);

    // Lock before updating shared resource
    lock_guard<mutex> lock(mtx);
    for (const auto& pair : localFreq) {
        globalFreq[pair.first] += pair.second;
    }
}

// Main function to handle file processing
void processFile(const string& filename, int numThreads) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    // Read entire file content into a vector
    vector<string> words;
    string word;
    while (file >> word) {
        words.push_back(word);
    }

    file.close();
    size_t totalWords = words.size();
    size_t segmentSize = totalWords / numThreads;

    vector<thread> threads;
    unordered_map<string, int> globalFreq;

    for (int i = 0; i < numThreads; i++) {
        // Define segment range
        size_t start = i * segmentSize;
        size_t end = (i == numThreads - 1) ? totalWords : (i + 1) * segmentSize;
        vector<string> segment(words.begin() + start, words.begin() + end);

        // Launch thread
        threads.emplace_back(threadFunction, segment, ref(globalFreq));
    }

    // Join all threads
    for (auto& t : threads) {
        t.join();
    }

    // Print final word frequencies
    cout << "\nFinal Word Frequency Count:\n";
    for (const auto& pair : globalFreq) {
        cout << pair.first << ": " << pair.second << endl;
    }
}

// Main function
int main() {
    string filename = "sample.txt";
    int numThreads = 4;

    processFile(filename, numThreads);
    return 0;
}
