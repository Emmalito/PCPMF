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
            Hedger hedger = new (testParameters, marketData[0], serverAddress);
            hedger.Hedge(marketData.Skip(1).ToArray());

            //CSV creation
            Console.WriteLine("PF value :");
            foreach(double val in hedger.PfValues)
            {
                Console.WriteLine(val);
            }
            Console.WriteLine("Option price:");
            foreach (double price in hedger.OptionPrices)
            {
                Console.WriteLine(price);
            }
        }

    }
}
