#pragma once

#include <fstream>
#include <mutex>
#include <string>
#include <chrono>

class TimestampLogger {
public:
    static TimestampLogger& getInstance() {
        static TimestampLogger instance;
        return instance;
    }

    void log_timestamp(const std::string& event_name, uint64_t timestamp) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!file_.is_open()) {
            file_.open("timestamps.csv", std::ios::out);
            if (!file_.is_open()) {
                return;
            }
            // 写入CSV头
            if (file_.tellp() == 0) {
                file_ << "Event,Timestamp(us)" << std::endl;
            }
        }
        file_ << event_name << ":" << timestamp;
        if(event_name == "PHY_Before" || event_name == "PUSCH task Finish_time"){
            file_ << std::endl;
        }
        else{
            file_ << ", ";
        }
    }

    void log_timestamp(const std::string& event_name1, uint64_t timestamp1, const std::string& event_name2, uint64_t timestamp2){
        std::lock_guard<std::mutex> lock(mutex_);
        if (!file_.is_open()) {
            file_.open("timestamps.csv", std::ios::out);
            if (!file_.is_open()) {
                return;
            }
            // 写入CSV头
            if (file_.tellp() == 0) {
                file_ << "Event,Timestamp(us)" << std::endl;
            }
        }
        file_ << event_name1 << ":" << timestamp1 << ", " << event_name2 << ":" << timestamp2 << ", " << "Duration" << ":" << timestamp2 - timestamp1 << std::endl;
    }

private:
    TimestampLogger() {}
    ~TimestampLogger() {
        if (file_.is_open()) {
            file_.close();
        }
    }

    std::ofstream file_;
    std::mutex mutex_;
}; 