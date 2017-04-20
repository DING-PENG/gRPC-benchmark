#include <iostream>
#include <string>
#include <memory>

#include <grpc++/grpc++.h>

#include "benchmark.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using benchmark::Data;

char str[1024 * 1024];

class BenchmarkImpl final : public benchmark::Benchmark::Service {
    Status GetData(ServerContext* context, const Data* req,
            Data* reply) override {
        //reply->CopyFrom(*req);
        reply->set_val(str);
        return Status::OK;
    }
};

void RunServer() {
    for (uint64_t i = 0; i < 1024 * 1024; i++)
        str[i] = '0';
    std::string server_address("0.0.0.0:50051");
    BenchmarkImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}
