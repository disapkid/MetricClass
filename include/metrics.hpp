#pragma once

#include <string>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <type_traits>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iostream>

class Metric {
public:
    virtual ~Metric() = default;
    virtual std::string get_value() const = 0;
    virtual void reset() = 0;
};

template <typename T>
class AtomicMetric : public Metric {
    static_assert(std::is_arithmetic_v<T>, "Arithmetic values allowed only");
private:
    std::atomic<T> value;
public:
    explicit AtomicMetric(T initial = T{}) : value(initial) {};

    void set_value(T val) { value.store(val, std::memory_order_relaxed); }

    void add_value(T delta) { 
        if constexpr (std::is_integral_v<T>) {
            value.fetch_add(delta, std::memory_order_relaxed);
        }   
        else {
            T current = value.load(std::memory_order_relaxed);
            while (!value.compare_exchange_weak(current, current + delta, 
                  std::memory_order_relaxed)) {}
        }
    }

    std::string get_value() const override {
        if(std::is_floating_point_v<T>) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << value.load(std::memory_order_relaxed);
            return oss.str();
        }
        else {
            return std::to_string(value.load(std::memory_order_relaxed));
        }
    }

    void reset() override {
        value.store(T{}, std::memory_order_relaxed);
    }
};

class MetricLogger {
private:
    std::unordered_map<std::string, std::unique_ptr<Metric>> map;
    std::mutex mutex;
public:
    template <typename T>
    void register_metric(const std::string& name, T initial = T{}) {
        std::lock_guard<std::mutex> lock(mutex);
        map[name] = std::make_unique<AtomicMetric<T>>(initial);
    }

    template <typename T>
    void update_and_add_metric(const std::string& name, T value) {
        std::lock_guard<std::mutex> lock(mutex);
        auto ptr = map.find(name);
        if(ptr != map.end()) {
            AtomicMetric<T>* temp = dynamic_cast<AtomicMetric<T>*>(ptr->second.get());
            if(temp) {
                temp->add_value(value);
            }
        }
        else return;
    }

    template <typename T>
    void update_and_set_metric(const std::string& name, T value) {
        std::lock_guard<std::mutex> lock(mutex);
        auto ptr = map.find(name);
        if(ptr != map.end()) {
            AtomicMetric<T>* temp = dynamic_cast<AtomicMetric<T>*>(ptr->second.get());
            if(temp) {
                temp->set_value(value);
            }
        }
        else return;
    }

    void write_to_file(const std::string& filename);
};