using FinancialApp.Portfolio;
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
        }

        [Test]
        public void CreationPortfolio()
        {
            Assert.That(Portfolio, Is.Not.Null);
        }

        [Test]
        public void ValueFieldCreationPortfolio()
        {
            Portfolio testPortfolio = new Portfolio(3, 10);
            double deltasLenght = testPortfolio.Deltas.Length;
            Assert.Multiple(() =>
            {
                Assert.That(testPortfolio.PfValue, Is.EqualTo(10));
                Assert.That(testPortfolio.Cash, Is.EqualTo(10));
                Assert.That(deltasLenght, Is.EqualTo(3));
            });
        }

        [Test]
        public void Rebalancing()
        {
            double[] newDeltas = new double[3] { 0.1, 0.1, 0.05 };
            Assert.Multiple(() =>
            {
                Assert.That(Portfolio.Deltas, Is.EqualTo(newDeltas));
                Assert.That(Portfolio.Cash, Is.EqualTo(0.75));
            });
        }

        [Test]
        public void ValueRebalancing()
        {
            double valueBeforeRebalancing = Portfolio.PfValue;
            double[] newDeltas = new double[3] { 0.4, 0.2, 0.1 };
            Dictionary<string, double> spotsList = new()
            {
                { "MC", 45 },
                { "SAN", 23 },
                { "ENX", 42 }
            };
            DateTime date = DateTime.Now;
            DataFeed dataFeed = new(date, spotsList);
            Portfolio.RebalancePortfolio(dataFeed, newDeltas);
            double valueAfterRebalancing = Portfolio.PfValue;
            Assert.That(valueAfterRebalancing, Is.EqualTo(valueBeforeRebalancing));
        }

        [Test]
        public void CashValueAfterUpdatePf()
        {
            double capitalizationTime = 0.003;
            double riskFreeRate = 0.05;
            Dictionary<string, double> spotsList = new()
            {
                { "MC", 50 },
                { "SAN", 20 },
                { "ENX", 45 }
            };
            DateTime date = DateTime.Now;
            DataFeed dataFeed = new(date, spotsList);
            Portfolio.UpdatePfValue(dataFeed, capitalizationTime, riskFreeRate);
            double newCash = 0.75 * Math.Exp(riskFreeRate*capitalizationTime);
            Assert.That(Portfolio.Cash, Is.EqualTo(newCash));
        }
    }
}