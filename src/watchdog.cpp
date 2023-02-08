#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

void log(string message)
{
    // if the log file does not exist, create it
    // if the log file exists, append the message to it
    ofstream log_file; // declare the log file
    log_file.open("log.txt", ios::app); // open the log file
    log_file << message << endl; // write the message to the log file
    log_file.close(); // close the log file
}

bool isProcessRunning(string process_name)
{
    // issue pidof -s <process_name> command and read the output into a string
    string command = "pidof -s " + process_name; // command to issue
    FILE *in; // declare the input stream
    char buff[512]; // declare the buffer
    if (!(in = popen(command.c_str(), "r"))) // issue the command
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
        return false; // return false
    }

    // if the output is not empty, then the process is running
    else
    {
        return true;
    }
}


bool lauchProcess(string process_name)
{
    // launch the process using the command: <process_name> & 
    string command = process_name + " &"; // command to issue
    system(command.c_str()); // issue the command

    // get the pid of the process
    command = "pidof -s " + process_name; // command to issue
    FILE *in; // declare the input stream
    char buff[512]; // declare the buffer
    if (!(in = popen(command.c_str(), "r"))) // issue the command
    {
        return 1; // if the command fails, return 1
    }
    string result = ""; // declare the result string
    while (fgets(buff, sizeof(buff), in) != NULL) // read the output into the buffer
    {
        result += buff; // append the buffer to the result string
    }
    pclose(in); // close the input stream

    // log the pid of the process
    log("Launched " + process_name + " with PID " + result); 

    return true;
}

int main()
{
    // list of processes to watch from config.txt file
    vector<string> processes; // declare the vector of processes
    ifstream config_file; // declare the config file
    config_file.open("config.txt"); // open the config file
    string line; // declare the line string
    while (getline(config_file, line)) // read each line into the line string
    {
        processes.push_back(line); // append the line to the vector of processes
    }
    config_file.close(); // close the config file

    // loop forever
    while (true) 
    {
        // check if each process is running
        for (int i = 0; i < processes.size(); i++) 
        {
            // if the process is not running, launch it
            if (!isProcessRunning(processes[i])) 
            {
                lauchProcess(processes[i]);
            }
            // if the process is running log it with PID
            else
            {
                string command = "pidof -s " + processes[i]; // command to issue
                FILE *in; // declare the input stream
                char buff[512]; // declare the buffer
                if (!(in = popen(command.c_str(), "r"))) // issue the command
                {
                    return 1; // if the command fails, return 1
                 }
                string result = ""; // declare the result string
                while (fgets(buff, sizeof(buff), in) != NULL) // read the output into the buffer
                {
                    result += buff; // append the buffer to the result string
                }
                pclose(in); // close the input stream
                log(processes[i] + " is running with PID " + result); 
            }
        }
        return 0;
    }
}
