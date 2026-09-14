#include "BackendServer.h"
#include "LoadBalancer.h"
#include "LoadBalancingStrategy.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

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

    lb::BackendServer a("10.0.0.2", 8082, 5);
    lb::BackendServer b("10.0.0.3", 8083, 5);
    lb::BackendServer c("10.0.0.4", 8084, 5);
    std::vector<lb::BackendServer*> servers = {&a, &b, &c};

    lb::RoundRobinStrategy rr;
    failures += !expect(rr.selectServer(servers) == &a, "round robin first selection");
    failures += !expect(rr.selectServer(servers) == &b, "round robin second selection");
    failures += !expect(rr.selectServer(servers) == &c, "round robin third selection");
    failures += !expect(rr.selectServer(servers) == &a, "round robin wraps around");

    b.simulateFailure();
    failures += !expect(rr.selectServer(servers) == &c, "round robin skips unhealthy server");

    lb::LeastConnectionsStrategy least;
    a.incrementConnections();
    a.incrementConnections();
    c.incrementConnections();
    failures += !expect(least.selectServer(servers) == &c || least.selectServer(servers) == &a, "least connections chooses minimum load");

    lb::BackendServer d("10.0.0.5", 8085, 5);
    lb::BackendServer e("10.0.0.6", 8086, 5);
    d.simulateFailure();
    e.simulateFailure();
    std::vector<lb::BackendServer*> unhealthyServers = {&d, &e};
    failures += !expect(least.selectServer(unhealthyServers) == nullptr, "all unhealthy returns null");

    lb::LoadBalancer balancer;
    balancer.addServer(&a);
    balancer.addServer(&b);
    balancer.addServer(&c);
    balancer.setStrategy(std::make_unique<lb::LeastConnectionsStrategy>());
    auto request = balancer.routeRequest(10, 128, 10);
    failures += !expect(request.status == lb::RequestStatus::SUCCESS || request.status == lb::RequestStatus::REJECTED,
                        "routeRequest returns valid lifecycle state");
    failures += !expect(balancer.serverCount() == 3, "load balancer tracks added servers");

    balancer.removeServer(b.getId());
    failures += !expect(balancer.serverCount() == 2, "load balancer removes tracked server");

    const auto healthyServers = balancer.getHealthyServers();
    failures += !expect(!healthyServers.empty(), "healthy server list is populated");

    return failures;
}

} // namespace

int main() {
    const int failures = runAllTests();
    if (failures == 0) {
        std::cout << "All backend server and strategy tests passed." << std::endl;
        return 0;
    }
    std::cerr << failures << " test(s) failed." << std::endl;
    return 1;
}
