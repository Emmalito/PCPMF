#include <iostream>
#include <string>
#include <fstream>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <nlohmann/json.hpp>

#include "pnl/pnl_matrix.h"
#include "pricing.grpc.pb.h"
#include "pricer.hpp"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

PnlMat* convertPastToPnlMat(const PricingInput *input) {
    // Find size
    int m, n;
    m = input->past_size();
    if (m == 0) {
        return NULL;
    }
    n = input->past(0).value_size();
    for (int i = 0; i < input->past_size(); i++) {
        const PastLines &pastLine = input->past(i);
        if (pastLine.value_size() !=n) {
            std::cerr << "size mismatch in past" << std::endl;
            return NULL;
        }
    }
    // Parse data
    PnlMat *past = pnl_mat_create(m, n);
    for (int i = 0; i < input->past_size(); i++) {
        const PastLines &pastLine = input->past(i);
        for (int j = 0; j < pastLine.value_size(); j++) {
            MLET(past, i, j ) = pastLine.value(j);
        }
    }
    return past;
}

// Logic and data behind the server's behavior.
class GrpcPricerImpl final : public GrpcPricer::Service {
public:
    BlackScholesPricer &pricer;
    GrpcPricerImpl(BlackScholesPricer &pricer) : pricer(pricer) {}

    Status PriceAndDeltas(ServerContext *context, const PricingInput *input, PricingOutput *output) override {
        double price, priceStdDev;
        PnlVect *delta, *deltaStdDev;
        bool isMonitoringDate = input->monitoringdatereached();
        double currentDate = input->time();
        PnlMat *past = convertPastToPnlMat(input);
        if (past == NULL) {
            return Status(grpc::StatusCode::INVALID_ARGUMENT, "Cannot read past");
        }
        pnl_mat_print(past);
        pricer.priceAndDeltas(past, currentDate, isMonitoringDate, price, priceStdDev, delta, deltaStdDev);
        output->set_price(price);
        output->set_pricestddev(priceStdDev);
        for (int i = 0; i < delta->size; i++) {
            output->add_deltas(GET(delta, i));
            output->add_deltasstddev(GET(deltaStdDev, i));
        }
        pnl_mat_free(&past);
        pnl_vect_free(&delta);
        pnl_vect_free(&deltaStdDev);
        return Status::OK;
    }

    Status Heartbeat(ServerContext *context, const Empty* input, ReqInfo *output) override {
        std::cout << "In heartbeat !" << std::endl;
        output->set_domesticinterestrate(pricer.interestRate);
        output->set_relativefinitedifferencestep(pricer.fdStep);
        output->set_samplenb(pricer.nSamples);
        return Status::OK;
    }
};

void RunServer(nlohmann::json &jsonParams) {
    std::string server_address("localhost:50051");
    BlackScholesPricer pricer(jsonParams);
    pricer.print();
    GrpcPricerImpl service(pricer);

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to a *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Exactly one argument is required." << std::endl;
        std::cout << "Usage: ./pricing_server math_params.json" << std::endl;
    }
    std::ifstream ifs(argv[1]);
    nlohmann::json jsonParams = nlohmann::json::parse(ifs);
    RunServer(jsonParams);

    return 0;
}
