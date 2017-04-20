#include <iostream>
#include <string>
#include <memory>
#include <chrono>

#include <grpc++/grpc++.h>

#include "benchmark.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using benchmark::Data;
using benchmark::Benchmark;

char str[1024 * 1024];

class Client {
    public:
        Client(std::shared_ptr<Channel> channel)
            : stub_(Benchmark::NewStub(channel)) {}
    
        void Test(uint64_t loop) {
            Data data;
            Data reply;
            data.set_val(str);

            std::cout << "Send begin" << std::endl;
            std::chrono::time_point<std::chrono::system_clock> start, end;
            start = std::chrono::system_clock::now();

            for (uint64_t i = 0; i < loop; i++) {
                ClientContext context;
                Status status = stub_->GetData(&context, data, &reply);
                if (!status.ok()) {
                    std::cout << status.error_code() << ": " << status.error_message() << std::endl;
                }
            }

            end = std::chrono::system_clock::now();
            std::cout << "Send finish" << std::endl;
            std::chrono::duration<double> elapsed_seconds = end-start;
            double avg = loop / elapsed_seconds.count();

            std::cout << "  Finish " << loop << " requests in "
                      << elapsed_seconds.count() << "s\n"
                      << "  Avg: " << avg << " requests/sec\n"
                      << "  Estimated bandwidth: "<< sizeof(char) * avg * 8 * 2 << " Mbps\n";
        }
    private:
        std::unique_ptr<Benchmark::Stub> stub_;
};

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " <address:port>" << std::endl;
        return 1;
    }
    
    std::cout << "Preparing data" << std::endl;
    for (uint64_t i = 0; i < 1024 * 1024; i++)
        str[i] = '0';

    Client client(grpc::CreateChannel(argv[1], grpc::InsecureChannelCredentials()));
    client.Test(1024);

    return 0;
}
