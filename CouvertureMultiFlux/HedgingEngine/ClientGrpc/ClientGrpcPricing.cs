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

        public PricingOutput PriceandDeltas(PricingInput input)
        {
            var client = new GrpcPricer.GrpcPricerClient(Channel);
            PricingOutput output = client.PriceAndDeltas(input);
            return output;
        }

        public ReqInfo Heartbeat()
        {
            var client = new GrpcPricer.GrpcPricerClient(Channel);
            ReqInfo output = client.Heartbeat( new Empty { } );
            return output;
        }

        public PricingInput ParamsToInput(DataFeed[] past, bool monitoringDateReached, double time)
        {
            IEnumerable<PastLines> matrixPast = new List<PastLines>();
            foreach(var pastLine in past)
            {
                PastLines line = new PastLines();
                line.Value.Add(pastLine);
                matrixPast.Append(line);
            }
            PricingInput input = new PricingInput();
            input.Past.Value.Add(matrixPast);
            input.MonitoringDateReached = monitoringDateReached;
            input.Time = time;
            return input;
        }
    }
}
