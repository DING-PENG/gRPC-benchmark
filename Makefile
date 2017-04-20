CXX = g++
CPPFLAGS = -I/usr/local/include -pthread
CPPFLAGS += -I$(HOME)/.local/include
CXXFLAGS = -std=c++11 -ggdb -Wall
LDFLAGS = -L$(HOME)/.local/lib -lgpr -lgrpc -lgrpc++ -lprotobuf -lpthread -ldl
PROTOC = $(HOME)/.local/bin/protoc
GRPC_CPP_PLUGIN = $(HOME)/.local/bin/grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

PROTOS_PATH = .

vpath %.proto $(PROTOS_PATH)

all: client server

client: benchmark.pb.o benchmark.grpc.pb.o client.o
	$(CXX) $^ $(LDFLAGS) -o $@

server: benchmark.pb.o benchmark.grpc.pb.o server.o
	$(CXX) $^ $(LDFLAGS) -o $@

.PRECIOUS: %.grpc.pb.cc
%.grpc.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=. $<
#%.pb.cc: %.proto
#	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=. --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

clean:
	rm -f *.o *.pb.cc *.pb.h client server
