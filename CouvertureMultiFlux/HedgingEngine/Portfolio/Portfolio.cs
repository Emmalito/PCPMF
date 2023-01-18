using MarketData;

namespace HedgingEngine.Portfolio
{
    public class Portfolio
    {
        public double PfValue { get; private set; }
        public double[] Deltas { get; private set;  }
        public double Cash { get; private set; }

        public Portfolio(int nbStocks, double cash = 0)
        {
            Deltas = new double[nbStocks];
            Cash = cash;
            PfValue = cash;
        }

        public void UpdatePfValue(DataFeed dataFeed, double riskFreeRate)
        {
            Cash *= riskFreeRate;
            PfValue = Cash + Utils.ScalarProduct(Deltas, dataFeed.SpotList.Values.ToArray());
        }

        public void RebalancePortfolio(DataFeed dataFeed, double[] newDeltas)
        {
            if (newDeltas.Length != Deltas.Length)
            {
                throw new ArgumentException("Vérifier le nombre d'éléments de vos tableaux !");
            }
            Deltas = newDeltas;
            Cash = PfValue - Utils.ScalarProduct(Deltas, dataFeed.SpotList.Values.ToArray());
        }
    }

    public class Utils
    {
        public static double ScalarProduct(double[] v1, double[] v2)
        {
            return v1.Zip(v2, (d, s) => d * s).Sum();
        }
    }
}
