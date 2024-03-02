#include "registry/service_manager.h"
#include <mutex>

namespace talko::registry {
bool ServiceManager::addService(const std::string& service_name, const std::string& ip, uint16_t port) {
    std::unique_lock<std::shared_mutex> lock(mtx_);

    auto iter = services_.find(service_name);
    if (iter != services_.end()) {
        return false;
    }

    NodeInfo info(ip, port);
    services_.insert({ service_name, info });
    return true;
}

bool ServiceManager::addMethod(const std::string& service_name, const std::string& method_name) {
    std::unique_lock<std::shared_mutex> lock(mtx_);

    auto iter_srv = services_.find(service_name);
    if (iter_srv == services_.end()) {
        return false;
    }

    auto iter_mtd = iter_srv->second.methods.find(method_name);
    if (iter_mtd != iter_srv->second.methods.end()) {
        return false;
    }

    iter_srv->second.methods.insert(method_name);
    return true;
}

bool ServiceManager::serviceExist(const std::string& service_name) {
    std::shared_lock<std::shared_mutex> lock(mtx_);

    auto iter = services_.find(service_name);
    return iter != services_.end();
}

bool ServiceManager::methodExist(const std::string& service_name, const std::string& method_name) {
    std::shared_lock<std::shared_mutex> lock(mtx_);

    auto iter_srv = services_.find(service_name);
    if (iter_srv == services_.end()) {
        return false;
    }

    auto iter_mtd = iter_srv->second.methods.find(method_name);
    return iter_mtd != iter_srv->second.methods.end();
}

void ServiceManager::removeService(const std::string& service_name) {
    std::unique_lock<std::shared_mutex> lock(mtx_);

    auto iter = services_.find(service_name);
    if (iter != services_.end()) {
        services_.erase(iter);
    }
}

void ServiceManager::removeMethod(const std::string& service_name, const std::string& method_name) {
    std::unique_lock<std::shared_mutex> lock(mtx_);

    auto iter_srv = services_.find(service_name);
    if (iter_srv == services_.end()) {
        return;
    }

    auto iter_mtd = iter_srv->second.methods.find(method_name);
    if (iter_mtd != iter_srv->second.methods.end()) {
        iter_srv->second.methods.erase(iter_mtd);
    }
}

std::optional<ServiceManager::ServiceInfo> ServiceManager::find(const std::string& service_name, const std::string& method_name) {
    ServiceInfo res;

    std::shared_lock<std::shared_mutex> lock(mtx_);

    auto iter_srv = services_.find(service_name);
    if (iter_srv == services_.end()) {
        return std::nullopt;
    }

    auto iter_mtd = iter_srv->second.methods.find(method_name);
    if (iter_mtd == iter_srv->second.methods.end()) {
        return std::nullopt;
    }

    res.first  = iter_srv->second.ip;
    res.second = iter_srv->second.port;

    return res;
}

} // namespace talko::registry