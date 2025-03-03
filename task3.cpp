#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <iomanip>
#include <map>

using namespace std;

struct Page {
    int page_number;
    unsigned char age_counter; // 8-bit counter
    Page(int num) : page_number(num), age_counter(0) {}
};

// Function to simulate the aging algorithm
int simulateAgingAlgorithm(const vector<int>& page_references, int num_frames) {
    unordered_map<int, Page*> page_table;
    queue<int> page_order;
    int page_faults = 0;

    for (size_t i = 0; i < page_references.size(); i++) {
        int page = page_references[i];

        // Age all existing pages
        for (auto& entry : page_table) {
            entry.second->age_counter >>= 1; // Right shift to age
        }

        // Check if page is already in memory
        if (page_table.find(page) != page_table.end()) {
            page_table[page]->age_counter |= 0x80; // Set MSB if referenced
        } else {
            page_faults++;

            if (page_table.size() >= num_frames) {
                // Find the page with the smallest aging counter
                int victim_page = -1;
                unsigned char min_age = 255;
                for (auto& entry : page_table) {
                    if (entry.second->age_counter < min_age) {
                        min_age = entry.second->age_counter;
                        victim_page = entry.first;
                    }
                }
                
                // Remove victim page
                if (victim_page != -1) {
                    delete page_table[victim_page];
                    page_table.erase(victim_page);
                }
            }

            // Add the new page
            Page* new_page = new Page(page);
            new_page->age_counter |= 0x80; // Set MSB
            page_table[page] = new_page;
        }
    }

    // Clean up
    for (auto& entry : page_table) {
        delete entry.second;
    }

    return page_faults;
}

int main() {
    string filename;
    cout << "Enter the input file name: ";
    cin >> filename;

    ifstream input_file(filename);
    if (!input_file) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    vector<int> page_references;
    int page;
    
    // Read page references from file
    while (input_file >> page) {
        page_references.push_back(page);
    }
    
    input_file.close();

    if (page_references.empty()) {
        cerr << "No page references found in the file." << endl;
        return 1;
    }

    // Output CSV file for plotting
    ofstream output_csv("page_faults.csv");
    output_csv << "Frames,Page Faults per 1000 References\n";

    cout << "\nSimulating the aging algorithm with different page frame counts...\n";

    // Vary the number of page frames from 1 to 20
    for (int num_frames = 1; num_frames <= 20; num_frames++) {
        int page_faults = simulateAgingAlgorithm(page_references, num_frames);
        double faults_per_1000 = (static_cast<double>(page_faults) / page_references.size()) * 1000;
        
        cout << "Frames: " << setw(2) << num_frames 
             << " | Page Faults per 1000 References: " 
             << fixed << setprecision(2) << faults_per_1000 << endl;

        output_csv << num_frames << "," << faults_per_1000 << "\n";
    }

    output_csv.close();
    cout << "\nResults saved to 'page_faults.csv'. You can plot this data using Python or Excel.\n";

    return 0;
}
