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
            /*if (args.Length != 1)
            {
                Console.WriteLine("Exactly one argument is required.");
                Console.WriteLine("Usage: ./financialapp test_parameters.json");
            }*/
            //Server Adresse
            string serverAddress = "http://localhost:50051";

            //Read the Test parameters
            string jsonString = File.ReadAllText("C:\\Users\\Emmalito\\OneDrive\\Bureau\\Projet_multi_flux\\test_params.json.txt");// args[1]);
            TestParameters testParameters = JsonIO.FromJson(jsonString);
            
            //Create and read the market data
            MarketInfo marketInfo = new ();
            double[] initialSpots = new double[4] { 8, 8.5, 9, 10 };
            double[] trends = new double[4] { 0.12, 0.03, 0.09, 0.06 };
            marketInfo.InitialSpots = initialSpots;
            marketInfo.Trends = trends;
            string filename = "marketData.csv";
            ShareValueGenerator.Create(testParameters, marketInfo, filename);
            List <DataFeed> marketData = MarketDataReader.ReadDataFeeds(filename);

            //Hedging
            IHedger hedger = new HedgerEngine (testParameters, marketData[0], serverAddress);
            hedger.Hedge(marketData.Skip(1).ToArray());

            //JSon creation
            string outputPath = "C:\\Users\\Emmalito\\OneDrive\\Bureau\\Projet_multi_flux\\PCPMF\\portfolio.json";
            using (FileStream fs = File.Create(outputPath));
            OutputData output = new();
            for (int idx = 0; idx < hedger.PfValues.Count; idx++)
            {
                output.OutputDate = hedger.Dates[idx];
                output.PortfolioValue = hedger.PfValues[idx];
                output.Delta = hedger.Deltas[idx];
                output.DeltaStdDev = hedger.DeltasStdDev[idx];
                output.Price = hedger.OptionPrices[idx];
                output.PriceStdDev = hedger.OptionPricesStdDev[idx];
                Console.WriteLine($"Diff Pf value et Option price = {hedger.PfValues[idx] - hedger.OptionPrices[idx]}");
                File.AppendAllText(outputPath, JsonIO.ToJson(output));
            }
        }
    }
}
