
using FinancialApp.ClientGrpc;

namespace TestHedgingEngine
{
    public class TestsClientGrpCPricing
    {
        public ClientGrpcPricing? Client;

        [SetUp]
        public void Setup()
        {
            Client = new();
        }

        [Test]
        public void TestIsCreate()
        {
            Assert.That(Client, Is.Not.Null);
        }
    }
}