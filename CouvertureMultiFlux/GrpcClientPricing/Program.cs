﻿using System.Threading.Tasks;
using Grpc.Net.Client;
using GrpcClientPricing;

// The port number must match the port of the gRPC server.
using var channel = GrpcChannel.ForAddress("http://localhost:50051");
var client = new GrpcPricer.GrpcPricerClient(channel);
var reply = await client.InfoAsync(
            new Empty { });
Console.WriteLine("Server response: " + reply.Mat);
Console.WriteLine("Press any key to exit...");
Console.ReadKey();