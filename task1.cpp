#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <fcntl.h>
#include <cstring>

using namespace std;

// Function to split input into command and arguments
vector<string> parse_command(const string& input) {
    vector<string> tokens;
    stringstream ss(input);
    string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to execute built-in commands
bool execute_builtin_command(const vector<string>& args) {
    if (args.empty()) return false;
    string command = args[0];

    if (command == "cd") {
        if (args.size() > 1) {
            if (chdir(args[1].c_str()) != 0) perror("cd failed");
        }
        else {
            cout << "Current directory: " << getcwd(nullptr, 0) << endl;
        }
        return true;
    }
    else if (command == "dir") {
        system(("ls " + (args.size() > 1 ? args[1] : "")).c_str());
        return true;
    }
    else if (command == "environ") {
        extern char** environ;
        for (char** env = environ; *env; ++env) {
            cout << *env << endl;
        }
        return true;
    }
    else if (command == "set" && args.size() >= 3) {
        setenv(args[1].c_str(), args[2].c_str(), 1);
        return true;
    }
    else if (command == "echo") {
        for (size_t i = 1; i < args.size(); i++) cout << args[i] << " ";
        cout << endl;
        return true;
    }
    else if (command == "pause") {
        cout << "Press Enter to continue...";
        cin.ignore();
        return true;
    }
    else if (command == "quit") {
        exit(0);
    }
    else if (command == "help") {
        cout << "\n===== Custom Shell Help Menu =====\n";
        cout << "cd [DIRECTORY]   - Change the current directory\n";
        cout << "dir [DIRECTORY]  - List files in a directory\n";
        cout << "environ          - Show all environment variables\n";
        cout << "set VAR VALUE    - Set an environment variable\n";
        cout << "echo [TEXT]      - Display text on the screen\n";
        cout << "pause            - Pause the shell until Enter is pressed\n";
        cout << "quit             - Exit the shell\n";
        cout << "help             - Show this help menu\n";
        cout << "COMMAND [ARGS]   - Run an external program\n";
        cout << "COMMAND < input  - Redirect input from a file\n";
        cout << "COMMAND > output - Redirect output to a file\n";
        cout << "COMMAND >> file  - Append output to a file\n";
        cout << "COMMAND &        - Run command in background\n";
        cout << "===================================\n";
        return true;
    }
    return false;
}

// Function to execute external commands with I/O redirection
void execute_external_command(vector<string> args) {
    bool background = (!args.empty() && args.back() == "&");
    if (background) args.pop_back();

    int input_fd = -1, output_fd = -1;
    vector<const char*> exec_args;

    for (size_t i = 0; i < args.size(); i++) {
        if (args[i] == "<" && i + 1 < args.size()) {
            input_fd = open(args[i + 1].c_str(), O_RDONLY);
            if (input_fd == -1) perror("Input redirection failed");
            i++;
        }
        else if (args[i] == ">" && i + 1 < args.size()) {
            output_fd = open(args[i + 1].c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output_fd == -1) perror("Output redirection failed");
            i++;
        }
        else if (args[i] == ">>" && i + 1 < args.size()) {
            output_fd = open(args[i + 1].c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (output_fd == -1) perror("Append redirection failed");
            i++;
        }
        else {
            exec_args.push_back(args[i].c_str());
        }
    }
    exec_args.push_back(nullptr);

    pid_t pid = fork();
    if (pid == 0) {  // Child process
        if (input_fd != -1) { dup2(input_fd, STDIN_FILENO); close(input_fd); }
        if (output_fd != -1) { dup2(output_fd, STDOUT_FILENO); close(output_fd); }

        execvp(exec_args[0], const_cast<char* const*>(exec_args.data()));
        perror("Execution failed");
        exit(1);
    }
    else if (pid > 0) {  // Parent process
        if (!background) waitpid(pid, nullptr, 0);
    }
    else {
        perror("Fork failed");
    }
}

// Function to process input command
void process_input(const string& input) {
    vector<string> args = parse_command(input);
    if (!execute_builtin_command(args)) {
        execute_external_command(args);
    }
}

// Function to read batch file and execute commands
void read_batch_file(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Unable to open batch file " << filename << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        process_input(line);
    }
    file.close();
}

// Main function
int main(int argc, char* argv[]) {
    if (argc > 1) {
        // Run in batch mode (read commands from a file)
        read_batch_file(argv[1]);
    }
    else {
        // Run interactive shell mode
        string input;
        while (true) {
            // Show the current directory in the prompt
            cout << getcwd(nullptr, 0) << " > ";
            cout.flush(); // Ensure prompt is displayed

            // Get user input
            if (!getline(cin, input)) {
                cout << "\nExiting shell...\n";
                break;
            }

            // Process the user command
            process_input(input);
        }
    }
    return 0;
}