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
        public Portfolio.Portfolio HedgePortfolio { get; set; }
        public IPricer Client { get; set; }
        public List<double> OptionPrices;
        public List<double> PfValues;
        public List<DateTime> Dates;
        private List<DataFeed> _past;

        public Hedger(TestParameters testParameters, DataFeed firstDataFeed, string serverAddress)
        {
            TestParameters = testParameters;
            _past = new();
            Client = new ClientGrpcPricing(serverAddress);
            OptionPrices = new List<double>();
            PfValues = new List<double>();
            Dates = new List<DateTime>();
            InitiateOracle();
            InitiatePortfolio(firstDataFeed);
        } 

        private void InitiateOracle()
        {
            var rebalancingOracleDescription = TestParameters.RebalancingOracleDescription;
            RebalanceOracle = new FixedOracle(rebalancingOracleDescription, TestParameters.PayoffDescription.CreationDate);
        }

        private void InitiatePortfolio(DataFeed firstDataFeed)
        {
            DataFeed[] past = GetDataFeed(firstDataFeed).ToArray();
            ComputePriceAndDelta(past);
            int nbStock = Client.Deltas.ToArray().Length;
            HedgePortfolio = new Portfolio.Portfolio(nbStock, Client.Price);
            HedgePortfolio.RebalancePortfolio(firstDataFeed, Client.Deltas.ToArray());
            if (RebalanceOracle.RebalancingTime(firstDataFeed.Date))
            {
                PfValues.Add(HedgePortfolio.PfValue);
                OptionPrices.Add(Client.Price);
                Dates.Add(firstDataFeed.Date);
            }
        }

        private void ComputePriceAndDelta(DataFeed[] past)
        {
            DateTime currentDate = past[^1].Date;
            MathDateConverter converter = new (TestParameters.NumberOfDaysInOneYear);
            double time = converter.ConvertToMathDistance(TestParameters.PayoffDescription.CreationDate, currentDate);
            bool monitoringDateReached = TestParameters.PayoffDescription.PaymentDates.Contains(currentDate);
            Client.ComputePriceAndDeltas(past, monitoringDateReached, time);
        }

        public void Hedge(DataFeed[] dataFeeds)
        {
            double riskFreeRate = TestParameters.AssetDescription.CurrencyRates[TestParameters.AssetDescription.DomesticCurrencyId];
            foreach (DataFeed dataFeed in dataFeeds)
            {
                ComputePriceAndDelta(GetDataFeed(dataFeed).ToArray());
                if (RebalanceOracle.RebalancingTime(dataFeed.Date))
                {
                    HedgePortfolio.UpdatePfValue(dataFeed, riskFreeRate);
                    HedgePortfolio.RebalancePortfolio(dataFeed, Client.Deltas.ToArray());
                    PfValues.Add(HedgePortfolio.PfValue);
                    OptionPrices.Add(Client.Price);
                    Dates.Add(dataFeed.Date);
                }
            }
        }

        private List<DataFeed> GetDataFeed(DataFeed newDataFeed)
        {
            List<DataFeed> past = new();
            foreach (DataFeed dataFeed in _past)
            {
                past.Add(dataFeed);
            }
            past.Add(newDataFeed);
            if (RebalanceOracle.RebalancingTime(newDataFeed.Date))
            {
                _past.Add(newDataFeed);
            }
            return past;
        }
    }
}
