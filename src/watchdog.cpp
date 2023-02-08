#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

void log(string message)
{
    // if the log file does not exist, create it
    // if the log file exists, append the message to it
    ofstream log_file;
    log_file.open("log.txt", ios::app);
    log_file << message << endl;
    log_file.close();
}

bool isProcessRunning(string process_name)
{
    // issue pidof -s <process_name> command and read the output into a string
    string command = "pidof -s " + process_name;
    FILE *in;
    char buff[512];
    if (!(in = popen(command.c_str(), "r")))
    {
        return 1;
    }
    string result = "";
    while (fgets(buff, sizeof(buff), in) != NULL)
    {
        result += buff;
    }
    pclose(in);

    // if the output is empty, then the process is not running
    if (result == "")
    {
        return false;
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
    string command = process_name + " &";
    system(command.c_str());

    // get the pid of the process
    command = "pidof -s " + process_name;
    FILE *in;
    char buff[512];
    if (!(in = popen(command.c_str(), "r")))
    {
        return 1;
    }
    string result = "";
    while (fgets(buff, sizeof(buff), in) != NULL)
    {
        result += buff;
    }
    pclose(in);

    // log the pid of the process
    log("Launched " + process_name + " with PID " + result);

    return true;
}

int main()
{
    // list of processes to watch from config.txt file
    vector<string> processes;
    ifstream config_file;
    config_file.open("config.txt");
    string line;
    while (getline(config_file, line))
    {
        processes.push_back(line);
    }
    config_file.close();

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
                string command = "pidof -s " + processes[i];
                FILE *in;
                char buff[512];
                if (!(in = popen(command.c_str(), "r")))
                {
                    return 1;
                }
                string result = "";
                while (fgets(buff, sizeof(buff), in) != NULL)
                {
                    result += buff;
                }
                pclose(in);
                log(processes[i] + " is running with PID " + result);
            }
        }
        return 0;
    }
}
