using MarketData;

namespace FinancialApp.ClientGrpc
{
    public interface IPricer
    {
        public double Price { get; }
        public double PriceStdDev { get; }
        public double[] Deltas { get; }
        public double[] DeltasStdDev { get; }
        public void ComputePriceAndDeltas(DataFeed[] past, bool monitoringDateReached, double time);
    }
}
