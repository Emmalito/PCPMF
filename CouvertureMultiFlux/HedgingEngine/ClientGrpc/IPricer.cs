using MarketData;

namespace HedgingEngine.ClientGrpc
{
    public interface IPricer
    {
        public double Price { get; }
        public double[] Deltas { get; }
        public void ComputePriceAndDeltas(DataFeed[] past, bool monitoringDateReached, double time);
    }
}
