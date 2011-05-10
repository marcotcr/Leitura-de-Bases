OBJECTS = client server slb.o
		

compile: $(OBJECTS)

client:	client.cc request.pb.cc
	g++ -o client client.cc request.pb.cc `pkg-config --cflags --libs protobuf` -lpthread -lzmq
server:	server.cc request.pb.cc slb.o
	g++ -o server server.cc slb.o request.pb.cc `pkg-config --cflags --libs protobuf` -lpthread -lzmq
request.pb.cc: request.proto
	protoc -I=./ --cpp_out=./ ./request.proto
slb.o: slb.cc slb.h
	g++ -c slb.cc request.pb.cc 
	

.PHONY: clean compile
		
clean:
	rm $(OBJECTS)
