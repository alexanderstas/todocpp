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
const std::string UNDONE    {"undone" };
const std::string HOME_DIR  {getenv("HOME")};
const std::string FILE_NAME {HOME_DIR + "/.todo.txt"};

bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(),
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

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
            std::istringstream(full_line.substr(last_colon_index+1, 1)) >> Done;

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

        bool get_Done() {
            return Done;
        }

        void toggle_Done(){
            Done = !Done;
        }

        void set_Done(bool done) {
            Done = done;
        }

        void print() {
            if (Done) {
                std::cout << ID << ": " << strikethrough(Task) << std::endl;
            }
            else {
                std::cout << ID << ": " << Task << std::endl;
            }
        }

        std::string prepare_write() {
            std::stringstream ss;
            ss << ID;
            ss << ":";
            ss << Task;
            ss << ":";
            ss << Done;
            ss << "\n";
            return ss.str();
        }

};

void read_entries(std::vector<Entry> &entries, std::string const &file_name) {
    std::ifstream infile(file_name.c_str());
    // Open file and read contents to the terminal
    std::string line;
    while(getline(infile, line)){
        Entry entry{line};
        entries.push_back(entry);
    }
    infile.close();
}

void write_entries(std::vector<Entry> &entries, const std::string &file_name) {
    std::ofstream outfile(file_name.c_str(), std::ios::out);
    for (auto &entry : entries) {
        outfile << entry.prepare_write();
    }
    outfile.close();    
}

int main(int argc, char *argv[]){
    
    srand(time(0));

    if (argc < 2) {
        std::cout 
            << "You must specify one of the following commands:\n"
            << "\tadd <name of task>                               | allows adding of a new task\n"
            << "\tdone <number of task when calling `todo all`>    | marks task with given number as done\n"
            << "\tundone <number of task when calling `todo all`>  | marks task with given number as undone\n"
            << "\tall                                              | lists all avaliable tasks\n"
            << "\treset                                            | deletes all tasks"
        << std::endl;
    } 

    if (argc == 2) {
        std::string argument(argv[1]);
        if (argument == RESET){
            int reset_password {(rand() % 900) + 100};
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
            std::vector<Entry> entries;
            read_entries(entries, FILE_NAME);
            for (auto& entry : entries) {
                entry.print();
            }
        }
        else{
            std::cout << "`" << argument << "` is not a valid command. type `todo` for valid commands.\n";
        }
    }
    else if (argc >= 3) {
        std::string argument(argv[1]);
        std::vector<Entry> entries;
        if (std::filesystem::exists(FILE_NAME.c_str())) {
            // Open file and read contents to the terminal
            read_entries(entries, FILE_NAME);
        }

        if (argument == ADD) {
            int last_entry_ID{0};

            if(entries.size() > 0) {
                last_entry_ID = entries.back().get_ID();
            }

            std::ofstream outfile(FILE_NAME.c_str(), std::ios::app);
            std::stringstream ss;
            for(int i=2; i<argc; i++) {
                ss << argv[i] << " ";
            }
            std::string task{ss.str()};
            Entry new_entry{last_entry_ID + 1, task};
            entries.push_back(new_entry);
            
            std::string output_string(new_entry.prepare_write());

            outfile << output_string;

            outfile.close();
            for (auto& entry : entries) {
                entry.print();
            }
            
        }
        else if (argument == DONE || argument == UNDONE) {
            std::vector<int> indices;
            int index;
            for(int i=2; i<argc; i++) {
                if (is_number(argv[i])) {
                    index = std::stoi(argv[i]);
                    indices.push_back(index);
                }
                else {
                    std::cout << "Non-integer entered." << std::endl;
                    break;
                }
            }

            if (argument == DONE) {
                for (auto& entry : entries) {
                    if(std::find(indices.begin(), indices.end(), entry.get_ID()) != indices.end()) {
                        entry.set_Done(true);
                    }
                }
            }
            else if (argument == UNDONE){
                for (auto& entry : entries) {
                    if(std::find(indices.begin(), indices.end(), entry.get_ID()) != indices.end()) {
                        entry.set_Done(false);
                    }
                }
            }
            write_entries(entries, FILE_NAME);
            for (auto& entry : entries) {
                entry.print();
            }
        }
    }
    else {
        std::cout << "Error: type `todo` for help." << std::endl;
    }
    return 0;
}
