using ParameterInfo.RebalancingOracleDescriptions;
using TimeHandler;

namespace HedgingEngine.Oracle
{
    public class FixedOracle : IRebalanceOracle
    {
        public int Period { get; set; }
        public DateTime CreationDate { get; set; }
        public int t { get; set; }
        //public List<DateTime> RebalancingTime { get; }
        public FixedTimesOracleDescription RebalancingOracleDescription { get; set; }

        public FixedOracle(FixedTimesOracleDescription rebalancingOracleDescription, DateTime creationDate)
        {
            Period = rebalancingOracleDescription.Period;
            CreationDate = creationDate;
            //RebalancingOracleDescription = rebalancingOracleDescription;
        }

        bool IRebalanceOracle.RebalancingTime(DateTime date)
        {
            t = DayCount.CountBusinessDays(CreationDate, date);
            return DayCount.CountBusinessDays(CreationDate, date) % Period == 0;
        }
    }
}
