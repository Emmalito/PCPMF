#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "pnl/pnl_matrix.h"
#include "pricing.grpc.pb.h"

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
    GrpcPricerImpl() {}

    Status Price(ServerContext *context, const PricingInput *input, PricingOutput *output) override {
        int index = input->index();
        bool monitoringDateReached = input->monitoringdatereached();
        double current_time = input->time();
        PnlMat *past = convertPastToPnlMat(input);
        if (past == NULL) {
            return Status(grpc::StatusCode::INVALID_ARGUMENT, "Cannot read past");
        }
        pnl_mat_print(past);
        output->set_price(current_time);
        output->add_deltas(1.);
        output->add_deltas(2.);
        pnl_mat_free(&past);
        return Status::OK;
    }

    Status Info(ServerContext *context, const Empty* input, ReqInfo *output) override {
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    GrpcPricerImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char **argv) {
    RunServer();

    return 0;
}
