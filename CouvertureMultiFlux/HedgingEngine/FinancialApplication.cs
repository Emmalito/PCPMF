using MarketData;
using ParameterInfo;
using ParameterInfo.JsonUtils;
using MarketDataGeneration;
using FinancialApp.Hedger;
using System.IO;
using System.Text;

namespace FinancialApp
{
    public class FinancialApplication
    {
        static void Main(string[] args)
        {
            if (args.Length != 3)
            {
                Console.WriteLine("Exactly three argument is required.");
                Console.WriteLine("Usage: ./financialapp Path\\To\\test_parameters.json Path\\To\\marketData.csv Path\\To\\portfolio.json");
            }
            //Server Adresse
            string serverAddress = "http://localhost:50051";

            //Read the Test parameters
            string jsonString = File.ReadAllText(args[0]);
            TestParameters testParameters = JsonIO.FromJson(jsonString);
            
            //Read the market data
            List <DataFeed> marketData = MarketDataReader.ReadDataFeeds(args[1]);

            //Hedging
            IHedger hedger = new HedgerEngine (testParameters, marketData[0], serverAddress);
            hedger.Hedge(marketData.Skip(1).ToArray());

            //JSon creation
            string outputPath = args[2];
            using (FileStream fs = File.Create(outputPath));
            List<OutputData> outputDatas = new();
            for (int idx = 0; idx < hedger.PfValues.Count; idx++)
            {
                OutputData output = new()
                {
                    OutputDate = hedger.Dates[idx],
                    PortfolioValue = hedger.PfValues[idx],
                    Delta = hedger.Deltas[idx],
                    DeltaStdDev = hedger.DeltasStdDev[idx],
                    Price = hedger.OptionPrices[idx],
                    PriceStdDev = hedger.OptionPricesStdDev[idx]
                };
                outputDatas.Add(output);
            }
            File.AppendAllText(outputPath, JsonIO.ToJson(outputDatas));
        }
    }
}
