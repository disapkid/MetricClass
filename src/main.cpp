#include <thread>
#include <random>
#include "metrics.hpp"

const std::string LOG_PATH = std::string(LOG_DIR) + "/metrics.log";

int main() {
    MetricLogger registry;
    
    registry.register_metric<double>("CPU Utilization");
    registry.register_metric<int>("HTTP Requests");
    registry.register_metric<int>("DB Queries");
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> cpu_dist(0.0, 2.0);
    std::poisson_distribution<int> http_dist(30);
    std::poisson_distribution<int> db_dist(15);
    
    for (int i = 0; i < 10; ++i) {
        registry.update_and_set_metric("CPU Utilization", cpu_dist(gen));
        registry.update_and_add_metric("HTTP Requests", http_dist(gen));
        registry.update_and_add_metric("DB Queries", db_dist(gen));

        registry.write_to_file(LOG_PATH);

        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}