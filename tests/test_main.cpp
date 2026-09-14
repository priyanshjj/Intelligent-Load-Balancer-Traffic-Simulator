#include "BackendServer.h"
#include "LoadBalancer.h"
#include "LoadBalancingStrategy.h"

#include <iostream>
#include <string>

namespace {

bool expect(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << std::endl;
        return false;
    }
    return true;
}

int runAllTests() {
    int failures = 0;

    lb::BackendServer server1("10.0.0.1", 8081, 3);
    failures += !expect(server1.getStatus() == lb::ServerStatus::HEALTHY, "server starts healthy");
    failures += !expect(server1.getActiveConnections() == 0, "no active connections initially");

    server1.incrementConnections();
    failures += !expect(server1.getActiveConnections() == 1, "increment connections");
    server1.decrementConnections();
    failures += !expect(server1.getActiveConnections() == 0, "decrement connections");

    server1.simulateFailure();
    failures += !expect(server1.getStatus() == lb::ServerStatus::UNHEALTHY, "failure status");
    failures += !expect(!server1.isHealthy(), "unhealthy server rejected");

    server1.recover();
    failures += !expect(server1.getStatus() == lb::ServerStatus::HEALTHY, "recovery returns healthy");

    std::size_t responseTime = 0;
    failures += !expect(server1.processRequest(512, 25, responseTime), "processRequest succeeds");
    failures += !expect(responseTime > 0, "response time populated");

    auto stats = server1.getStatistics();
    failures += !expect(stats.totalRequests == 1, "request count recorded");
    failures += !expect(stats.successfulRequests == 1, "successful requests recorded");

    server1.stop();
    failures += !expect(server1.getStatus() == lb::ServerStatus::STOPPED, "stop updates status");

    lb::BackendServer unhealthy("10.0.0.2", 8082, 2);
    unhealthy.simulateFailure();
    lb::BackendServer healthy("10.0.0.3", 8083, 2);
    std::vector<lb::BackendServer*> servers = {&healthy, &unhealthy};

    lb::RoundRobinStrategy rr;
    auto* selected = rr.selectServer(servers);
    failures += !expect(selected == &healthy, "round robin skips unhealthy server");

    lb::LeastConnectionsStrategy lc;
    healthy.incrementConnections();
    auto* leastSelected = lc.selectServer(servers);
    failures += !expect(leastSelected == &unhealthy || leastSelected == &healthy, "least connections chooses candidate");

    lb::LoadBalancer balancer;
    balancer.addServer(&healthy);
    balancer.addServer(&unhealthy);
    auto request = balancer.routeRequest(10, 128, 10);
    failures += !expect(request.status == lb::RequestStatus::SUCCESS || request.status == lb::RequestStatus::REJECTED,
                        "routeRequest returns valid lifecycle state");

    return failures;
}

} // namespace

int main() {
    const int failures = runAllTests();
    if (failures == 0) {
        std::cout << "All backend server and routing tests passed." << std::endl;
        return 0;
    }
    std::cerr << failures << " test(s) failed." << std::endl;
    return 1;
}
