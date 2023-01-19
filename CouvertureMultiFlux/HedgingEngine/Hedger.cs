using GrpcClientPricing;
using HedgingEngine.ClientGrpc;
using HedgingEngine.Oracle;
using MarketData;
using ParameterInfo;
using TimeHandler;

namespace HedgingEngine
{
    public class Hedger
    {
        public IRebalanceOracle RebalanceOracle { get; set; }
        public TestParameters TestParameters { get; set; }
        public List<DataFeed> DataFeeds { get; set; }
        public Portfolio.Portfolio HedgePortfolio { get; set; }
        public IPricer Client { get; set; }
        public List<double> OptionPrices;
        public List<double> PfValues;
        public List<DateTime> Dates;

        public Hedger(TestParameters testParameters, List<DataFeed> dataFeeds, string serverAddress)
        {
            TestParameters = testParameters;
            DataFeeds = dataFeeds;
            Client = new ClientGrpcPricing(serverAddress);
            OptionPrices = new List<double>();
            PfValues = new List<double>();
            Dates = new List<DateTime>();
            InitiateOracle();
            InitiatePortfolio();
        } 

        private void InitiateOracle()
        {
            var rebalancingOracleDescription = TestParameters.RebalancingOracleDescription;
            RebalanceOracle = new FixedOracle(rebalancingOracleDescription, TestParameters.PayoffDescription.CreationDate);
        }

        private void InitiatePortfolio()
        {
            DataFeed[] past = new DataFeed[1] { DataFeeds[0] };
            ComputePriceAndDelta(past);
            int nbStock = Client.Deltas.ToArray().Length;
            HedgePortfolio = new Portfolio.Portfolio(nbStock, OptionPrices[0]);
            HedgePortfolio.RebalancePortfolio(DataFeeds[0], Client.Deltas.ToArray());
            PfValues.Add(HedgePortfolio.PfValue);
        }

        private void ComputePriceAndDelta(DataFeed[] past)
        {
            DateTime currentDate = past[^1].Date;
            MathDateConverter converter = new (TestParameters.NumberOfDaysInOneYear);
            double time = converter.ConvertToMathDistance(TestParameters.PayoffDescription.CreationDate, currentDate);
            bool monitoringDateReached = TestParameters.PayoffDescription.PaymentDates.Contains(currentDate);
            Client.ComputePriceAndDeltas(past, monitoringDateReached, time);
            OptionPrices.Add(Client.Price);
            Dates.Add(currentDate);
        }

        public void Hedge(DataFeed[] dataFeeds)
        {
            double riskFreeRate = TestParameters.AssetDescription.CurrencyRates[TestParameters.AssetDescription.DomesticCurrencyId];
            List<DataFeed> past = new() { dataFeeds[0] };
            foreach (DataFeed dataFeed in dataFeeds)
            {
                past.Add(dataFeed);
                ComputePriceAndDelta(past.ToArray());
                HedgePortfolio.UpdatePfValue(dataFeed, riskFreeRate);
                PfValues.Add(HedgePortfolio.PfValue);
                if (RebalanceOracle.RebalancingTime(dataFeed.Date))
                {
                    HedgePortfolio.RebalancePortfolio(dataFeed, Client.Deltas.ToArray());
                }
            }
        }
    }
}
