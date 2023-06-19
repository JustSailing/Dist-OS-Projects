#include <iostream>
#include <sstream>

#include "logger.h"

Logger::Logger(std::string_view log_file, std::string_view csv_file)
{
    m_tid_main = std::this_thread::get_id();
    m_log_file = static_cast<std::string>(log_file);
    m_log_file_stream.open(m_log_file, std::ios::app);
    // Error Handling to check if open file worked
    // exit upon failbit being set
    if (m_log_file_stream.fail())
    {
        // This exit is needed. If the log file can not be opened
        // it would be difficult to keep track of index server info
        std::cout << "Log File: " << log_file << " could not be opened for logging\n";
        exit(2);
    }
    m_log_csv = static_cast<std::string>(csv_file);
    m_csv_file_stream.open(m_log_csv, std::ios::app);
    if (m_csv_file_stream.fail())
    {
        std::cout << "Csv File: " << csv_file << " could not be opened for logging\n";
        exit(2);
    }
}

void Logger::Log(std::string_view event) noexcept
{
    auto time_ = time(0);
    auto time_stamp = ctime(&time_);  
    // std::ostringstream ss;
    // ss << event << time_stamp << '\n';
    // auto log = ss.str();
    // Using a lock_guard that automatically releases lock when going
    // out of scope
    const std::lock_guard<std::mutex> lock(m_log_lock);
    // Writes event to file and flush buffer
    m_log_file_stream << event << time_stamp ;;
    m_log_file_stream.flush();
    // m_log_file_stream.bad() // check if flush works
}

void Logger::Log(std::string_view event, std::string_view message_type,
                 const std::chrono::system_clock::time_point& start,
                 const std::chrono::system_clock::time_point& end) noexcept
{
    // time_point needed to be converted to time_t in order for 
    // stringstream to even work
    auto t_start = std::chrono::system_clock::to_time_t(start);
    auto t_end = std::chrono::system_clock::to_time_t(end);
    auto t_difference = std::chrono::duration<double, std::nano>(end - start).count();
    {
        // TODO: Might need to use better locking system probably using semaphores would be better
        // but not sure
        const std::lock_guard<std::mutex> lock(m_log_lock);
        // Writes event to file and flush buffer
        m_log_file_stream << event << " [TIME STAMP] "
            << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())
            << " [START TIME] " << t_start
            << " [FINISH TIME] " << t_end
            << " [DIFF] " << t_difference << '\n';
    }
    const std::lock_guard<std::mutex> lock(m_csv_lock);
    // Writes message_type and times to file and flush buffer
    m_csv_file_stream << message_type << " , " << t_start << ", " << t_end << ", " << t_difference << '\n';
    m_csv_file_stream.flush();
}

bool Logger::CloseLog() noexcept
{
    // the main thread should be the one to close file streams
    // or the original thread who created the first instance of the Logger 
    if (m_tid_main == std::this_thread::get_id())
    {
        m_log_file_stream.flush();
        m_csv_file_stream.flush();
        m_log_file_stream.close();
        m_csv_file_stream.close();
        return true;
    }
    // a thread rather than the main thread called CloseLog.
    return false;
}