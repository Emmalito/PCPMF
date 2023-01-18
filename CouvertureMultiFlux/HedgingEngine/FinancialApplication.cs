using MarketData;
using ParameterInfo;
using ParameterInfo.JsonUtils;
using MarketDataGeneration;

namespace HedgingEngine
{
    public class FinancialApplication
    {
        static void Main(string[] args)
        {
            if (args.Length != 1)
            {
                Console.WriteLine("Exactly one argument is required.");
                Console.WriteLine("Usage: ./financialapp test_parameters.json");
            }
            //Server Adresse
            string serverAddress = "http://0.0.0.0:50051";

            //Read the Test parameters
            string jsonString = File.ReadAllText(args[1]);
            TestParameters testParameters = JsonIO.FromJson(jsonString);
            
            //Create and read the market data
            MarketInfo marketInfo = new ();
            double[] initialSpots = new double[3] { 8, 8.5, 9 };
            double[] trends = new double[3] { 0.12, 0.03, 0.09 };
            marketInfo.InitialSpots = initialSpots;
            marketInfo.Trends = trends;
            string filename = "marketData.csv";
            ShareValueGenerator.Create(testParameters, marketInfo, filename);
            List <DataFeed> marketData = MarketDataReader.ReadDataFeeds(filename);


            //Hedging
            Hedger hedger = new Hedger(testParameters, marketData, serverAddress);
            hedger.Hedge(marketData.Skip(1).ToArray());

            //CSV creation

        }

    }
}
