#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <algorithm>

using namespace std;

// global variables
vector<string> watched_processes; // declare the vector of processes

// function prototypes
vector<string> splitString(string str, char delimiter);
vector<string> getProcesses();
bool updateConfig();
void log(string message);
pid_t getProcessPID(string process_name);
bool lauchProcess(string process_name);
string getStartupParameters(string process_name);
void pediodicCheck(vector<string> processes, int interval_ms);
void updateProcessLog(vector<string> processes);

vector<string> splitString(string str, char delimiter)
{
    // split a string into a vector of strings
    // based on the delimiter
    vector<string> result;                 // declare the result vector
    string temp = "";                      // declare the temp string
    for (int i = 0; i < str.length(); i++) // iterate through the string
    {
        if (str[i] == delimiter) // if the character is the delimiter
        {
            result.push_back(temp); // append the temp string to the result vector
            temp = "";              // reset the temp string
        }
        else // if the character is not the delimiter
        {
            temp += str[i]; // append the character to the temp string
        }
    }
    result.push_back(temp); // append the temp string to the result vector
    return result;          // return the result vector
}

bool updateConfig(){
    // source config dir: ../config/config.txt
    // destination config dir: config.txt

    // read the config file
    ifstream config_file;              // declare the config file
    config_file.open("../config/config.txt");    // open the config file
    string line;                       // declare the line string
    string config = "";
    while (getline(config_file, line)) // read each line into the line string
    {
        config += line + "\n";
    }

    // check if destination config file exists
    ifstream destination_config_file;              // declare the config file
    destination_config_file.open("config.txt");    // open the config file  
    string destination_config = "";
    while (getline(destination_config_file, line)) // read each line into the line string
    {
        destination_config += line + "\n";
    }

    // if the config file is different, update it
    if (config != destination_config){
        ofstream destination_config_file;              // declare the config file
        destination_config_file.open("config.txt");    // open the config file
        destination_config_file << config;
        destination_config_file.close();
        return true;
    }

    return false;
}


vector<string> getProcesses()
{
    bool isUpdated = updateConfig();
    // list of processes to watch from config.txt file
    vector<string> processes;          // declare the vector of processes
    ifstream config_file;              // declare the config file
    config_file.open("config.txt");    // open the config file
    string line;                       // declare the line string
    while (getline(config_file, line)) // read each line into the line string
    {
        // if the line is empty, skip it
        if (line == "") // if the line is empty
        {
            continue; // skip it
        }

        // if the line is a comment, skip it
        if (line[0] == '#') // if the line is a comment
        {
            continue; // skip it
        }

        // split the line into a vector of strings
        vector<string> line_split = splitString(line, ' ');

        processes.push_back(line_split[0]); // append the line to the vector of processes

        if (isUpdated){
            updateProcessLog(processes);
        }
    }
    config_file.close(); // close the config file
    return processes;
}

void log(string message)
{
    // if the log file does not exist, create it
    // if the log file exists, append the message to it
    ofstream log_file;                  // declare the log file
    log_file.open("../logs/log.txt", ios::app); // open the log file

    // get current date/time, format is YYYY-MM-DD.HH:mm:ss
    time_t now = time(0);
    char dt[80];
    strftime(dt, sizeof(dt), "%Y-%m-%d|%X", localtime(&now));

    log_file << dt << " ";              // write the date and time to the log file
    log_file << message << endl;        // write the message to the log file
    log_file.close();                   // close the log file
}

pid_t getProcessPID(string process_name){
    // issue pidof -s <process_name> command and read the output into a string
    string command = "pidof -s " + process_name; // command to issue
    FILE *in;                                    // declare the input stream
    char buff[512];                              // declare the buffer
    if (!(in = popen(command.c_str(), "r")))     // issue the command
    {
        return 1; // if the command fails, return 1
    }
    string result = "";
    while (fgets(buff, sizeof(buff), in) != NULL) // read the output into the buffer
    {
        result += buff; // append the buffer to the result string
    }
    pclose(in); // close the input stream

    // if the output is empty, then the process is not running
    if (result == "") // if the result string is empty
    {
        // display the output in the terminal
        cout << process_name << " is not running" << endl;
        return -1; // return false
    }

    // if the output is not empty, then the process is running
    else
    {
        // display the output in the terminal
        cout << process_name << " is running with PID " << result << endl;
        return stoi(result);
    }
}

bool lauchProcess(string process_name)
{
    // launch the process in a new thread
    // and append the stdout and stderr to the log file <process_name>.log
    // make sure that the log is appended to the log file and not overwritten

    // if the process is already running, do not launch it
    if (getProcessPID(process_name) > 0)
    {
        return false;
    }

    // if the process is not running, launch it
    else
    {
        // get the startup parameters for the process
        string startup_parameters = getStartupParameters(process_name);

        // launch the process
        string command = "nohup " + process_name + " " + startup_parameters + " >> ../logs/" + process_name + ".log 2>&1 &";
        cout << command << endl;
        system(command.c_str());

        // wait for the process to start
        while (!getProcessPID(process_name) > 0)
        {
            this_thread::sleep_for(chrono::milliseconds(100));
        }

        // log the process launch
        log(process_name + " launched" + " with PID " + to_string(getProcessPID(process_name)));

        return true;
    }
}

string getStartupParameters(string process_name)
{
    // get the startup parameters for the process from config.txt file
    ifstream config_file;              // declare the config file
    config_file.open("config.txt");    // open the config file
    string line;                       // declare the line string
    while (getline(config_file, line)) // read each line into the line string
    {
        // if the line is empty, skip it
        if (line == "") // if the line is empty
        {
            continue; // skip it
        }

        // if the line is a comment, skip it
        if (line[0] == '#') // if the line is a comment
        {
            continue; // skip it
        }

        // split the line into a vector of strings
        vector<string> line_split = splitString(line, ' ');

        if (line_split[0] == process_name)
        {
            if (line_split.size() > 1)
            {
                return line_split[1];
            }
            else
            {
                return "";
            }
        }
    }
    config_file.close(); // close the config file
    return "";
}

void pediodicCheck(int interval_ms){
    // check if the processes are running every interval_ms milliseconds
    // if the process is not running, launch it
    while (true)
    {
        vector<string> processes = getProcesses();

        for (int i = 0; i < processes.size(); i++)
        {
            lauchProcess(processes[i]);
        }
        this_thread::sleep_for(chrono::milliseconds(interval_ms));
    }
}

void updateProcessLog(vector<string> processes){
    // for each process in the processes vector
    // check if its included in the watched_processes vector
    // if it is not, add it to the watched_processes vector
    // and log the process
    for (int i = 0; i < processes.size(); i++)
    {
        if (find(watched_processes.begin(), watched_processes.end(), processes[i]) == watched_processes.end())
        {
            watched_processes.push_back(processes[i]);
            log("Watching " + processes[i]);
        }
    }

    // for each process in the watched_processes vector
    // check if its included in the processes vector
    // if it is not, remove it from the watched_processes vector
    // and log the process
    for (int i = 0; i < watched_processes.size(); i++)
    {
        if (find(processes.begin(), processes.end(), watched_processes[i]) == processes.end())
        {
            watched_processes.erase(watched_processes.begin() + i);
            log("Stopped watching " + watched_processes[i]);
        }
    }
}