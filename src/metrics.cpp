#include "metrics.hpp"

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_tm = *std::localtime(&now_time_t);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();
    return oss.str();
}

void MetricLogger::write_to_file(const std::string& filename) {
    std::unordered_map<std::string, std::string> snapshot;
    {
        std::lock_guard<std::mutex> lock(mutex);
        for(auto& [name, metric ] : map) {
            snapshot[name] = metric->get_value();
            metric->reset();
        }
    }

    std::ofstream file(filename, std::ios::app);
    if (!file) return;

    file << getCurrentTimestamp();
    for (auto& [name, value] : snapshot) {
        file << " \"" << name << "\" " << value;
    }
    file << "\n";
}