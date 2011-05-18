OBJECTS = client server

compile: $(OBJECTS)

client:	client.cc request.pb.cc slb_message.pb.cc z_utils.o
	g++ -o client client.cc request.pb.cc z_utils.o slb_message.pb.cc `pkg-config --cflags --libs protobuf` -lpthread -lzmq
server:	server.cc request.pb.cc slb.o slb_message.pb.cc z_utils.o 
	g++ -o server server.cc slb.o z_utils.o request.pb.cc slb_message.pb.cc `pkg-config --cflags --libs protobuf` -lpthread -lzmq
request.pb.cc: request.proto
	protoc -I=./ --cpp_out=./ ./request.proto
slb_message.pb.cc: slb_message.proto
	protoc -I=./ --cpp_out=./ ./slb_message.proto
slb.o: slb.cc slb.h z_utils.o
	g++ -c slb.cc request.pb.cc slb_message.pb.cc z_utils.cc
z_utils.o: z_utils.h z_utils.cc
	g++ -c z_utils.cc
	

.PHONY: clean compile
		
clean:
	rm $(OBJECTS)
