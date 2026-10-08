#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <filesystem>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// Globals or constants for builtins
const std::unordered_set<std::string> BUILTINS = {"exit", "echo", "type", "pwd", "cd"};

// Splits a string into separate arguments
std::vector<std::string> splitArguments(const std::string &input) {
    std::vector<std::string> args;
    std::string current_arg = "";
    bool in_single_quotes = false;
    bool in_double_quotes = false;
    bool escaped = false;
    bool has_content = false;

    for (size_t i = 0; i < input.length(); ++i) {
        char c = input[i];

        if(escaped) {
            current_arg += c;
            has_content = true;
            escaped = false;
            continue;
        }

        if (in_single_quotes) {
            if (c == '\'') {
                in_single_quotes = false;
            } else {
                current_arg += c;
                has_content = true;
            }
        } else if (in_double_quotes) {
            if (c == '"') {
                in_double_quotes = false;
            } else if (c == '\\') {
                if (i + 1 < input.length()) {
                    char next_c = input[i + 1];
                    if (next_c == '"' || next_c == '\\' || next_c == '$') {
                        current_arg += next_c;
                        has_content = true;
                        i++;
                    } else {
                        current_arg += c;
                        has_content = true;
                    }
                } else {
                    current_arg += c;
                    has_content = true;
                }
            } else {
                current_arg += c;
                has_content = true;
            }
        } else {
            if (c == '\\') {
                escaped = true;
                has_content = true;
            } else if (c == '\'') {
                in_single_quotes = true;
                has_content = true;
            } else if (c == '"') {
                in_double_quotes = true;
                has_content = true;
            } else if ( c == ' ' || c == '\t') {
                if (has_content) {
                    args.push_back(current_arg);
                    current_arg = "";
                    has_content = false;
                }
            } else {
                current_arg += c;
                has_content = true;
            }
        }
    }

    if (has_content) {
        args.push_back(current_arg);
    }

    return args;
}

// Locates an executable file in the system PATH
std::string findInPath(const std::string &command) {
    const char* path_env = getenv("PATH");
    if (!path_env) return "";

    std::istringstream ss(path_env);
    std::string directory;
    while (getline(ss, directory, ':')) {
        std::string full_path = directory + "/" + command;
        if (access(full_path.c_str(), X_OK) == 0) {
            return full_path;
        }
    }
    return "";
}

// HANDLERS FOR BUILTIN COMMANDS
void handleEcho(const std::vector<std::string>& args) {
    for (size_t i = 1; i < args.size(); ++i) {
        std::cout << args[i] << (i + 1 < args.size() ? " " : "");
    }
    std::cout << '\n';
}

void handleType(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cerr << "type: missing argument\n";
        return;
    }
    
    const std::string& command_arg = args[1];

    if (BUILTINS.count(command_arg)) {
        std::cout << command_arg << " is a shell builtin\n";
        return;
    }

    std::string full_path = findInPath(command_arg);
    if (!full_path.empty()) {
        std::cout << command_arg << " is " << full_path << '\n';
        return;
    }

    std::cout << command_arg << ": not found\n";
}

void handlePwd() {
    std::cout << std::filesystem::current_path().string() << '\n';
}

void handleCd(const std::vector<std::string>& args) {
  std::string target_path;

  if (args.size() < 2) {
    const char* home_env = getenv("HOME");
    if (home_env) {
      target_path = home_env;
    } else {
      std::cerr << "cd: HOME not set\n";
      return;
    }
  } else {
    target_path = args[1];
  }

  if (!target_path.empty() && target_path[0] == '~') {
    const char* home_env = getenv("HOME");
    if (home_env) {
      target_path = std::string(home_env) + target_path.substr(1);
    }
  }
  
  std::error_code ec;
  std::filesystem::current_path(target_path, ec);

  if (ec) {
    std::cerr << "cd: " << target_path << ": No such file or directory\n";
  }
}

// HANDLER FOR EXTERNAL BINARIES
void executeExternal(const std::string& path, std::vector<std::string>& args, const std::string& redirect_file) {
    std::vector<char*> c_args;
    for (const auto &arg : args) {
        c_args.push_back(const_cast<char*>(arg.c_str()));
    }
    c_args.push_back(nullptr); 

    pid_t pid = fork();
    if (pid == 0) {
        if (!redirect_file.empty()) {
            int fd = open(redirect_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                std::cerr << "Failed to open redirect file\n";
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        execv(path.c_str(), c_args.data());
        std::cerr << "Failed to execute " << args[0] << '\n';
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
    } else {
        std::cerr << "Fork failed\n";
    }
}

// Helper to manage builtin redirection
struct RedirectionGuard {
    int saved_stdout = -1;
    bool active = false;

    RedirectionGuard(const std::string& file) {
        if (!file.empty()) {
            saved_stdout = dup(STDOUT_FILENO); // Save terminal output descriptor
            int fd = open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd >= 0) {
                dup2(fd, STDOUT_FILENO);
                close(fd);
                active = true;
            }
        }
    }

    ~RedirectionGuard() {
        if (active && saved_stdout != -1) {
            dup2(saved_stdout, STDOUT_FILENO); // Restore terminal output
            close(saved_stdout);
        }
    }
};

int main() {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    while (true) {
        std::cout << "$ ";
        std::string command_line;
        if (!std::getline(std::cin, command_line)) {
            break; 
        }

        if (command_line.empty()){
            continue;
        }

        std::vector<std::string> args = splitArguments(command_line);
        if (args.empty()) {
            continue;
        }

        
        std::string base_command = args[0];

        std::string redirect_file = "";
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == ">" || args[i] == "1>") {
                if (i + 1 < args.size()) {
                    redirect_file = args[i + 1];
                    args.erase(args.begin() + i, args.begin() + i + 2);
                }
                break;
            }
        }

        // 1. Core Builtin Router (Protected by the guard)
        {
            RedirectionGuard guard(redirect_file);
            if (base_command == "exit") {
                break;
            } else if (base_command == "echo") {
                handleEcho(args);
                continue;
            } else if (base_command == "type") {
                handleType(args);
                continue;
            } else if (base_command == "pwd") {
                handlePwd();
                continue;
            } else if (base_command == "cd") {
                handleCd(args);
                continue;
            }
        }

        // 2. External Programs Router
        std::string executable_path = findInPath(base_command);
        if (!executable_path.empty()) {
            executeExternal(executable_path, args, redirect_file);
        } else {
            std::cout << base_command << ": command not found\n";
        }
    }
    return 0;
}
