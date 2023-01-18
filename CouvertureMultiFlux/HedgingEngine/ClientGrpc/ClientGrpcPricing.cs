using Grpc.Net.Client;
using GrpcClientPricing;
using MarketData;
using System.Threading.Channels;

namespace HedgingEngine.ClientGrpc
{
    public class ClientGrpcPricing
    {
        public GrpcChannel Channel { get; set; }

        public ClientGrpcPricing(string serverAddress= "http://localhost:50051")
        {
            Channel = GrpcChannel.ForAddress(serverAddress);
        }

        public PricingOutput PriceandDeltas(DataFeed[] past, bool monitoringDateReached, double time)
        {
            var client = new GrpcPricer.GrpcPricerClient(Channel);
            PricingInput input = ParamsToInput(past, monitoringDateReached, time);
            PricingOutput output = client.PriceAndDeltas(input);
            return output;
        }

        public ReqInfo Heartbeat()
        {
            var client = new GrpcPricer.GrpcPricerClient(Channel);
            ReqInfo output = client.Heartbeat( new Empty { } );
            return output;
        }

        private PricingInput ParamsToInput(DataFeed[] past, bool monitoringDateReached, double time)
        {
            PricingInput input = new PricingInput();
            foreach (var pastLine in past)
            {
                PastLines line = new();
                foreach (double price in pastLine.SpotList.Values.ToArray())
                {
                    line.Value.Add(price);
                }
                input.Past.Add(line);
            }
            input.MonitoringDateReached = monitoringDateReached;
            input.Time = time;
            return input;
        }
    }
}
