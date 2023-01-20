namespace FinancialApp.Oracle
{
    public interface IRebalanceOracle
    {
        bool RebalancingTime(DateTime date);
    }
}
