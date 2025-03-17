#include <iostream>
#include <filesystem>
#include <map>
#include <iomanip>

namespace fs = std::filesystem;

void scan_directory(const fs::path& directory, std::map<size_t, int>& histogram, size_t bin_width) {
    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            size_t file_size = entry.file_size();
            size_t bin = file_size / bin_width;
            histogram[bin]++;
        }
    }
}

void print_histogram(const std::map<size_t, int>& histogram, size_t bin_width) {
    std::cout << "File Size Histogram:\n";
    for (const auto& [bin, count] : histogram) {
        std::cout << "[" << bin * bin_width << " - " << (bin + 1) * bin_width - 1 << "] : ";
        std::cout << std::string(count, '*') << " (" << count << " files)\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <directory> <bin_width>\n";
        return 1;
    }

    fs::path start_directory = argv[1];
    size_t bin_width = std::stoul(argv[2]);

    if (!fs::exists(start_directory) || !fs::is_directory(start_directory)) {
        std::cerr << "Error: Invalid directory path.\n";
        return 1;
    }

    std::map<size_t, int> histogram;
    scan_directory(start_directory, histogram, bin_width);
    print_histogram(histogram, bin_width);

    return 0;
}
