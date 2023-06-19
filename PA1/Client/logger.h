//
// Created by william on 2/22/23.
//

#ifndef CLIENT_LOGGER_H
#define CLIENT_LOGGER_H

#include <mutex>
#include <fstream>
class Logger
{
public:
    // Logger constructor takes a filename and creates a logger file that keeps track
    // of all client activites
    Logger(const char *file);
    // main function used to log events
    void Log(std::string event);
    void LogClose(){m_log_file.close();};
    ~Logger() = default;

private:
    // File that the client should write to
    std::ofstream m_log_file;

};



#endif //CLIENT_LOGGER_H
