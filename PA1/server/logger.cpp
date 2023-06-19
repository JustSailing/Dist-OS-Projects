
#include "logger.h"
std::mutex filewrite;
Logger::Logger(const char *file)
{
    this->m_log_file.open(file, std::ios::app);
}

void Logger::Log(std::string event)
{
    filewrite.lock();
    this->m_log_file.write(event.c_str(), event.length());
    this->m_log_file.flush();
    filewrite.unlock();
}