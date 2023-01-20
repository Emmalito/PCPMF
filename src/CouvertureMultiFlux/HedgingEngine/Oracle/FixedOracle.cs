using ParameterInfo.RebalancingOracleDescriptions;
using TimeHandler;

namespace FinancialApp.Oracle
{
    public class FixedOracle : IRebalanceOracle
    {
        public int Period { get; set; }
        public DateTime CreationDate { get; set; }
        public int t { get; set; }
        private int _counter;
        public FixedTimesOracleDescription RebalancingOracleDescription { get; set; }

        public FixedOracle(FixedTimesOracleDescription rebalancingOracleDescription, DateTime creationDate)
        {
            Period = rebalancingOracleDescription.Period;
            CreationDate = creationDate;
            _counter = 0;
        }

        bool IRebalanceOracle.RebalancingTime(DateTime date)
        {
            _counter++;
            if (_counter == Period)
            {
                _counter = 0;
                return true;
            }
            return false;
        }
    }
}
