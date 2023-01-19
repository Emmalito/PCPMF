using Grpc.Net.Client;
using GrpcClientPricing;
using MarketData;

namespace FinancialApp.ClientGrpc
{
    public class ClientGrpcPricing : IPricer
    {
        public GrpcChannel Channel { get; set; }
        public double Price { get; private set; }
        public double PriceStdDev { get; private set; }
        public double[] Deltas { get; private set; }
        public double[] DeltasStdDev { get; private set; }

        public ClientGrpcPricing(string serverAddress= "http://localhost:50051")
        {
            Channel = GrpcChannel.ForAddress(serverAddress);
            Price = 0;
            PriceStdDev = 0;
        }

        public void ComputePriceAndDeltas(DataFeed[] past, bool monitoringDateReached, double time)
        {
            PricingOutput output = PriceandDeltas(past, monitoringDateReached, time);
            Price = output.Price;
            PriceStdDev = output.PriceStdDev;
            Deltas = output.Deltas.ToArray();
            DeltasStdDev = output.DeltasStdDev.ToArray();
        }

        private PricingOutput PriceandDeltas(DataFeed[] past, bool monitoringDateReached, double time)
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

        private static PricingInput ParamsToInput(DataFeed[] past, bool monitoringDateReached, double time)
        {
            PricingInput input = new ();
            foreach (var pastLine in past)
            {
                PastLines line = new ();
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
