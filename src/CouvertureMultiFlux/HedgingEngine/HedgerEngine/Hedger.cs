using GrpcClientPricing;
using FinancialApp.ClientGrpc;
using FinancialApp.Oracle;
using MarketData;
using ParameterInfo;
using TimeHandler;

namespace FinancialApp.Hedger
{
    public class HedgerEngine : IHedger
    {
        public IRebalanceOracle RebalanceOracle { get; set; }
        public TestParameters TestParameters { get; set; }
        public Portfolio.Portfolio HedgePortfolio { get; set; }
        public IPricer Client { get; set; }
        public List<double> OptionPrices { get; private set; }
        public List<double> OptionPricesStdDev { get; }
        public List<double[]> Deltas { get; }
        public List<double[]> DeltasStdDev { get; }
        public List<double> PfValues { get; private set; }
        public List<DateTime> Dates { get; private set; }
        private List<DataFeed> _past;

        public HedgerEngine(TestParameters testParameters, DataFeed firstDataFeed, string serverAddress)
        {
            TestParameters = testParameters;
            _past = new();
            Client = new ClientGrpcPricing(serverAddress);
            OptionPrices = new List<double>();
            OptionPricesStdDev = new List<double>();
            Deltas = new List<double[]>();
            DeltasStdDev = new List<double[]>();
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
            ComputePriceAndDelta(GetDataFeed(firstDataFeed).ToArray());
            int nbStock = Client.Deltas.ToArray().Length;
            HedgePortfolio = new Portfolio.Portfolio(nbStock, Client.Price);
            HedgePortfolio.RebalancePortfolio(firstDataFeed, Client.Deltas.ToArray());
            AddInfo(firstDataFeed.Date);
        }

        private void ComputePriceAndDelta(DataFeed[] past)
        {
            DateTime currentDate = past[^1].Date;
            MathDateConverter converter = new(TestParameters.NumberOfDaysInOneYear);
            double time = converter.ConvertToMathDistance(TestParameters.PayoffDescription.CreationDate, currentDate);
            bool monitoringDateReached = TestParameters.PayoffDescription.PaymentDates.Contains(currentDate);
            Client.ComputePriceAndDeltas(past, monitoringDateReached, time);
        }

        public void Hedge(DataFeed[] dataFeeds)
        {
            MathDateConverter converter = new(TestParameters.NumberOfDaysInOneYear);
            DateTime lastDate = Dates[0];
            double riskFreeRate = TestParameters.AssetDescription.CurrencyRates[TestParameters.AssetDescription.DomesticCurrencyId];
            foreach (DataFeed dataFeed in dataFeeds)
            {
                ComputePriceAndDelta(GetDataFeed(dataFeed).ToArray());
                double time = converter.ConvertToMathDistance(lastDate, dataFeed.Date);
                HedgePortfolio.UpdatePfValue(dataFeed, time,  riskFreeRate);
                lastDate = dataFeed.Date;
                if (RebalanceOracle.RebalancingTime(dataFeed.Date))
                {
                    HedgePortfolio.RebalancePortfolio(dataFeed, Client.Deltas.ToArray());
                    AddInfo(dataFeed.Date);
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
            if (TestParameters.PayoffDescription.PaymentDates.Contains(newDataFeed.Date))
            {
                _past.Add(newDataFeed);
            }
            return past;
        }

        private void AddInfo(DateTime date)
        {
            OptionPrices.Add(Client.Price);
            OptionPricesStdDev.Add(Client.PriceStdDev);
            Deltas.Add(Client.Deltas);
            DeltasStdDev.Add(Client.DeltasStdDev);
            PfValues.Add(HedgePortfolio.PfValue);
            Dates.Add(date);
        }
    }
}
