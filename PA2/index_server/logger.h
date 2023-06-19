#ifndef LOGGER_H
#define LOGGER_H

#include <mutex>
#include <fstream>
#include <thread>
#include <chrono>

/*
 *  Logger for index server
 *  Allows index server to write events to files on its storage to help keep track of 
 *  all events handled on behalf of peer nodes
 */

class Logger
{
public:
    // Using string_view for Logger constructor and Log function,
    // according to the cpp reference this provides read-only access to an
    // existing string. In other words this helps not create unneeded allocations
    Logger(std::string_view log_file, std::string_view csv_file);

    // Logs events made by index server with just time stamp to m_log_file
    // Example: socket creation, bind, listen, accept
    void Log(std::string_view event) noexcept;

    // Logs events made by index server with time stamp, start time, end time,
    // and the difference
    void Log(std::string_view event, std::string_view message_type,
             const std::chrono::system_clock::time_point &start,
             const std::chrono::system_clock::time_point &end) noexcept;

    // Closes the ofstream.
    // Note: Should only be called by the main process (not any thread)
    [[nodiscard]] bool CloseLog() noexcept;

    ~Logger() = default;

private:
    // Output file stream that the index server uses to write to file
    std::ofstream m_log_file_stream;

    // File to log all index server events
    std::string m_log_file;

    // Output file stream that the index server uses to log time differences in csv style
    std::ofstream m_csv_file_stream;

    // csv file for easier way to help with the evaluation part of the assignment
    // append to csv file.
    // Should avoid errors so need to add a boolean to if error from reading or sending
    // message_type start_time finish_time time_difference error
    std::string m_log_csv;

    // In the last assignment I made the lock global but I do not think that it
    // is necessary since the lock should only be accessed in Log and CloseLog
    // I might add a conditional variable
    // Log file Lock
    std::mutex m_log_lock;
    std::mutex m_csv_lock;

    // The tid of the main thread. Needed so no other thread other than the main
    // should close the file streams. Since the main thread makes sures all threads are
    // finished before closing the file streams
    std::thread::id m_tid_main;
};

#endif