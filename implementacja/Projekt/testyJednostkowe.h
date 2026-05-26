class testyJednostkowe{
public:
    int getPassedTestCounter();
    void setPassedTestCounter(int i);
    int getFailedTestCounter();
    void setFailedTestCounter(int i);
    void testujLogowanie();
    bool assertEqualsBool(bool a, bool b);
    testy();


private:
    int passedTestCounter = 0;
    int failedTestCounter = 0;
};
