using HedgingEngine.Portfolio;
using MarketData;
using NUnit.Framework;

namespace TestHedgingEngine.TestPortfolio
{
    [TestFixture]
    public class TestsPortfolio
    {
        public Portfolio Portfolio;

        [SetUp]
        public void Setup()
        {
            Portfolio = new Portfolio(3, 10);
        }

        [Test]
        public void CreationPortfolio()
        {
            Assert.That(Portfolio, Is.Not.Null);
        }

        [Test]
        public void ValueFieldCreationPortfolio()
        {
            Assert.That(Portfolio.PfValue, Is.EqualTo(10));
            Assert.That(Portfolio.Cash, Is.EqualTo(10));
            double deltasLenght = Portfolio.Deltas.Length;
            Assert.That(deltasLenght, Is.EqualTo(3));
        }

        [Test]
        public void Rebalancing()
        {
            double[] newDeltas = new double[3] { 0.1, 0.1, 0.05 };
            Dictionary<string, double> spotsList = new()
            {
                { "MC", 50 },
                { "SAN", 20 },
                { "ENX", 45 }
            };
            DateTime date = DateTime.Now;
            DataFeed dataFeed = new(date, spotsList);
            Portfolio.RebalancePortfolio(dataFeed, newDeltas);
            Assert.That(Portfolio.Deltas, Is.EqualTo(newDeltas));
            Assert.That(Portfolio.Cash, Is.EqualTo(0.75));
        }

        [Test]
        public void UpdatePf()
        {
            Rebalancing();
            double riskFreeRate = 0.05;
            Dictionary<string, double> spotsList = new()
            {
                { "MC", 55 },
                { "SAN", 25 },
                { "ENX", 50 }
            };
            DateTime date = DateTime.Now;
            DataFeed dataFeed = new(date, spotsList);
            Portfolio.UpdatePfValue(dataFeed, riskFreeRate);
            double newCash = 0.75 * riskFreeRate;
            double newPfValue = 10.5375;
            Assert.Multiple(() =>
            {
                Assert.That(Portfolio.Cash, Is.EqualTo(newCash));
                Assert.That(Portfolio.PfValue, Is.EqualTo(newPfValue));
            });
        }
    }
}