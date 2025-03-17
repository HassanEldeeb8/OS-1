#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

void list_files(const std::string& dir_path) {
    if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
        std::cerr << "Error: " << dir_path << " is not a valid directory." << std::endl;
        return;
    }

    std::cout << "Contents of directory: " << dir_path << std::endl;
    for (const auto& entry : fs::directory_iterator(dir_path)) {
        std::cout << entry.path().filename().string() << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <directory1> [directory2] ..." << std::endl;
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        list_files(argv[i]);
    }

    return 0;
}
