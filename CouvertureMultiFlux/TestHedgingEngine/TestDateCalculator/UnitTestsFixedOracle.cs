using HedgingEngine.Oracle;
using ParameterInfo.RebalancingOracleDescriptions;

namespace TestHedgingEngine
{
    [TestFixture]
    public class TestsOracle
    {
        public IRebalanceOracle Oracle;

        [SetUp]
        public void Setup()
        {
            DateTime creationDate = new(2023, 01, 16);
            FixedTimesOracleDescription description = new()
            {
                Period = 4
            };
            Oracle = new FixedOracle(description, creationDate);
        }

        [Test]
        public void CreationOracle()
        {
            Assert.That(Oracle, Is.Not.Null);
        }

        [Test]
        public void RebalancementDay()
        {
            DateTime day1 = new(2023, 01, 20);
            DateTime day2 = new(2023, 01, 26);
            DateTime day3 = new(2023, 02, 01);
            Assert.Multiple(() =>
            {
                Assert.That(Oracle.RebalancingTime(day1), Is.True);
                Assert.That(Oracle.RebalancingTime(day2), Is.True);
                Assert.That(Oracle.RebalancingTime(day3), Is.True);
            });
        }

        [Test]
        public void NoRebalancementDay()
        {
            DateTime day1 = new(2023, 01, 17);
            DateTime day2 = new(2023, 01, 18);
            DateTime day3 = new(2023, 01, 19);
            Assert.Multiple(() =>
            {
                Assert.That(Oracle.RebalancingTime(day1), Is.False);
                Assert.That(Oracle.RebalancingTime(day2), Is.False);
                Assert.That(Oracle.RebalancingTime(day3), Is.False);
            });
        }
    }
} 