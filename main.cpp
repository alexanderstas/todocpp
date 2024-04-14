#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <cstdio>

/*
 * Need to implement:
 *  1. .todo.txt file creator
 *  2. Parse command line args
 *  3. Write to file
 *  4. Read from file 
 */

// Entries are of the form:
//    12:Do Homework

const std::string RESET     {"reset"  };
const std::string DONE      {"done"   };
const std::string ADD       {"add"    };
const std::string ALL       {"all"    };
const std::string HOME_DIR  {getenv("HOME")};
const std::string FILE_NAME {HOME_DIR + "/.todo.txt"};

class Entry {
    private:
        int ID;
        std::string Task;
        bool Done;
        
    public:
        Entry(std::string full_line) {
            // parse full_line into id and task
            full_line.erase(std::remove(full_line.begin(), full_line.end(), '\n'), full_line.end());
            size_t first_colon_index = full_line.find_first_of(":");
            size_t last_colon_index = full_line.find_last_of(":");
            ID = std::stoi(full_line.substr(0, first_colon_index));
            Task = full_line.substr(first_colon_index+1, last_colon_index - first_colon_index - 1);
            std::istringstream(full_line.substr(last_colon_index+1)) >> Done;
        }
        Entry(int id, std::string task) : ID(id), Task(task) {
            Done = false;
        }

        int get_ID() {
            return ID;
        }

        std::string get_Task(){
            return Task;
        }
        
};

std::string strikethrough(const std::string& text) 
{
    std::string result;

    for (auto ch : text) 
    {
        result.append(u8"\u0336");
        result.push_back(ch);
    }
    return result;
}

int main(int argc, char *argv[]){
    
    srand(time(0));

    if (argc < 2) {
        std::cout 
            << "You must specify one of the following commands:\n"
            << "\tadd <name of task>                            | allows adding of a new task\n"
            << "\tdone <number of task when calling `todo all`> | marks task with given number as done\n"
            << "\tall                                           | lists all avaliable tasks\n"
            << "\treset                                         | deletes all tasks"
        << std::endl;
    } 

    if (argc == 2) {
        std::string argument(argv[1]);
        if (argument == RESET){
            int reset_password {rand() % 1000};
            int response;
            std::cout << "Are you sure you want to reset all tasks? If so, enter " << reset_password << ": ";
            std::cin >> response;
            if (response == reset_password) {
                // delete all todos
                std::remove(FILE_NAME.c_str());
                std::cout << "Erased todo history.\n";
            }
            else{
                std::cout << "Incorrect number. Did not erase todo history.\n";
            }
        }
        else if (argument == ALL){
            std::ifstream file(FILE_NAME.c_str());
            // Open file and read contents to the terminal
            std::string line;
            while(getline(file, line)){
                std::cout << line << std::endl;
            }
            file.close();
        }
        else{
            std::cout << "`" << argument << "` is not a valid command. type `todo` for valid commands.\n";
        }
    }
    else if (argc >= 3) {
        std::string argument(argv[1]);
        if (argument == ADD) {
            int last_entry_ID{0};
            if (std::filesystem::exists(FILE_NAME.c_str())) {
                std::ifstream infile(FILE_NAME.c_str());
                // Open file and read contents to the terminal
                std::string line;
                std::vector<Entry> entries;
                while(getline(infile, line)){
                    Entry entry{line};
                    entries.push_back(entry);
                }

                last_entry_ID = entries.back().get_ID();
                infile.close();
            }

            std::ofstream outfile(FILE_NAME.c_str(), std::ios::app);
            outfile << last_entry_ID + 1;
            outfile << ":";

            for(int i=2; i<argc; i++) {
                outfile << argv[i] << " ";
            }

            outfile << ":";
            outfile << "0";
            outfile << "\n";
            outfile.close();
            
        }
    }
    return 0;
}
