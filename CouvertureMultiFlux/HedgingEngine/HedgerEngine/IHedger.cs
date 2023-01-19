using MarketData;

namespace FinancialApp.Hedger
{
    public interface IHedger
    {
        public List<double> OptionPrices { get; }
        public List<double> OptionPricesStdDev { get; }
        public List<double[]> Deltas { get; }
        public List<double[]> DeltasStdDev { get; }
        public List<double> PfValues { get;  }
        public List<DateTime> Dates { get; }
        public void Hedge(DataFeed[] dataFeeds);
    }
}
