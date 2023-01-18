namespace HedgingEngine.Oracle
{
    public interface IRebalanceOracle
    {
        bool RebalancingTime(DateTime date);
    }
}
