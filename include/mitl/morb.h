/**
 * @file morb.h
 * @author Abdulelah Mulla
 * @date 11/24/2025
 */

#pragma once

#include <map>
#include <string>

/**
 * @brief Message bus for objects that need to know.
 * 
 * This is not uORB, this is Morb; the M is for Mulla.
 * This Message bus is susceptible to type errors if the
 * use passes the wrong type!
 * 
 * TODO: Re-design to avoid dynamic allocation
 * TODO: Make it asynchronous 
 */
class Morb {
private:
    std::map<std::string, std::function<void(const void*)>> _subscribers;
public:
    template<typename T>
    void subscribe(const std::string& topic, std::function<void(const T&)> callback) {
        _subscribers[topic] = [callback](const void* data) {
            const T* typed = static_cast<const T*>(data);
            callback(*typed);
        }
    }

    template<typename T>
    void publish(const std::string& topic, const T& msg) {
        auto it = _subscribers.find(topic);
        if (it != _subscribers.end()) {
            it->second(static_cast<const void*>(&msg));
        }
    }
};