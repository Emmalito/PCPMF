using GrpcClientPricing;
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
        public ClientGrpc.ClientGrpcPricing Client { get; set; }
        private List<double> OptionPrices;
        private List<double> PfValues;
        private List<DateTime> Dates;

        public Hedger(TestParameters testParameters, List<DataFeed> dataFeeds, string serverAddress)
        {
            TestParameters = testParameters;
            DataFeeds = dataFeeds;
            Client = new ClientGrpc.ClientGrpcPricing(serverAddress);
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
            PricingOutput output = ComputePriceAndDelta(past);
            int nbStock = output.Deltas.Length;
            HedgePortfolio = new Portfolio.Portfolio(nbStock, OptionPrices[0]);
            HedgePortfolio.RebalancePortfolio(DataFeeds[0], output.Deltas);
            PfValues.Add(HedgePortfolio.PfValue);
        }

        private PricingOutput ComputePriceAndDelta(DataFeed[] past)
        {
            //DateTime Maturity = TestParameters.PayoffDescription.PaymentDates.Last();
            DateTime currentDate = past[^1].Date;
            MathDateConverter converter = new (TestParameters.NumberOfDaysInOneYear);
            double time = converter.ConvertToMathDistance(TestParameters.PayoffDescription.CreationDate, currentDate);
            bool monitoringDateReached = TestParameters.PayoffDescription.PaymentDates.Contains(currentDate);
            PricingInput inputs = Client.ParamsToInput(past, monitoringDateReached, time);
            PricingOutput output = Client.PriceandDeltas(inputs);
            OptionPrices.Add(output.Price);
            Dates.Add(currentDate);
            return output;
        }

        public void Hedge(DataFeed[] dataFeeds)
        {
            double riskFreeRate = TestParameters.AssetDescription.CurrencyRates[TestParameters.AssetDescription.DomesticCurrencyId];
            List<DataFeed> past = new List<DataFeed>();
            past.Add(dataFeeds[0]);
            foreach (DataFeed dataFeed in dataFeeds)
            {
                past.Add(dataFeed);
                PricingOutput output = ComputePriceAndDelta(past.ToArray());
                HedgePortfolio.UpdatePfValue(dataFeed, riskFreeRate);
                PfValues.Add(HedgePortfolio.PfValue);
                if (RebalanceOracle.RebalancingTime(dataFeed.Date))
                {
                    HedgePortfolio.RebalancePortfolio(dataFeed, output.Deltas);
                }
            }
        }
    }
}
