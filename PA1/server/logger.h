
#ifndef SERVER_LOGGER_H
#define SERVER_LOGGER_H

#include <mutex>
#include <fstream>
class Logger
{
public:
    // Logger constructor takes a filename and creates a logger file that keeps track
    // of all server activites (Almost all not sure I'm going to log server socket inits, binding and etc)
    Logger(const char *file);
    void Log(std::string event);
    void LogClose(){m_log_file.close();};
    ~Logger() = default;

private:
    // File that the server should write to
    std::ofstream m_log_file;

    // for multithreading uses. writes should be mutually exclusive
    // counting semaphore initialized to 0 and this allows only for
    // one thread to write to the log file at once
    // Note: did not work just used a regular lock
};


#endif //CS550_03_HADDAD_WILLIAM_PA1_LOGGER_H
