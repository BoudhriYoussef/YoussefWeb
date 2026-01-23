//+------------------------------------------------------------------+
//|                                                  MartingaleEA.mq5 |
//|                                                                  |
//|                                                                  |
//+------------------------------------------------------------------+
#property copyright ""
#property version   "1.00"
#property description "Martingale EA with ATR entry condition"

//--- Input parameters
input double InitialLotSize = 0.01;          // Initial lot size
input int    DistancePips = 50;              // Distance between trades in pips (for averaging down)
input double ATRThreshold = 3.00;            // ATR threshold for entry
input int    TakeProfitPips = 100;           // Take profit in pips
input int    ATRPeriod = 14;                 // ATR period
input int    MagicNumber = 12345;            // Magic number for trades
input int    MaxTrades = 10;                 // Maximum number of trades in martingale sequence

//--- Global variables
double pipValue;
int atrHandle;
double lotSizes[];
int tradeCount = 0;
double totalVolume = 0;
double averagePrice = 0;
double firstTradePrice = 0;                  // Price of the first trade in sequence

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
{
    //--- Calculate pip value based on symbol
    pipValue = SymbolInfoDouble(_Symbol, SYMBOL_TRADE_TICK_SIZE) * 
               SymbolInfoDouble(_Symbol, SYMBOL_TRADE_CONTRACT_SIZE);
    
    if(SymbolInfoInteger(_Symbol, SYMBOL_DIGITS) == 3 || 
       SymbolInfoInteger(_Symbol, SYMBOL_DIGITS) == 5)
        pipValue *= 10;
    
    //--- Create ATR indicator handle
    atrHandle = iATR(_Symbol, PERIOD_CURRENT, ATRPeriod);
    if(atrHandle == INVALID_HANDLE)
    {
        Print("Error creating ATR indicator");
        return(INIT_FAILED);
    }
    
    //--- Initialize lot sizes array for martingale
    ArrayResize(lotSizes, MaxTrades);
    lotSizes[0] = InitialLotSize;
    
    for(int i = 1; i < MaxTrades; i++)
    {
        lotSizes[i] = lotSizes[i-1] * 2; // Martingale: double the lot size
    }
    
    return(INIT_SUCCEEDED);
}

//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
{
    if(atrHandle != INVALID_HANDLE)
        IndicatorRelease(atrHandle);
}

//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
{
    //--- Check for existing positions
    CheckForTP();
    
    int positions = CountPositions();
    
    //--- Only check ATR when no positions are open
    if(positions == 0)
    {
        //--- Reset trade count when no positions exist
        if(tradeCount > 0)
        {
            tradeCount = 0;
            totalVolume = 0;
            averagePrice = 0;
            firstTradePrice = 0;
        }
        
        CheckForEntry();
    }
    else
    {
        //--- When positions exist, check for additional entries when price moves down
        CheckForMartingaleEntry();
    }
}

//+------------------------------------------------------------------+
//| Check for entry condition (only when no positions exist)         |
//+------------------------------------------------------------------+
void CheckForEntry()
{
    double atrValue = GetATRValue();
    
    if(atrValue <= ATRThreshold && tradeCount == 0)
    {
        OpenBuyPosition(lotSizes[tradeCount]);
        tradeCount++;
        firstTradePrice = SymbolInfoDouble(_Symbol, SYMBOL_ASK); // Store first trade price
        Print("First trade opened at price: ", firstTradePrice);
    }
}

//+------------------------------------------------------------------+
//| Check for martingale entry when price moves down                 |
//+------------------------------------------------------------------+
void CheckForMartingaleEntry()
{
    if(tradeCount >= MaxTrades) return;
    
    double currentPrice = SymbolInfoDouble(_Symbol, SYMBOL_ASK);
    
    //--- Calculate how many distance levels we've moved down from first trade
    double priceDiffPoints = (firstTradePrice - currentPrice) / SymbolInfoDouble(_Symbol, SYMBOL_POINT);
    double priceDiffPips = priceDiffPoints / 10;
    
    //--- Calculate how many trades should be open based on distance moved
    int expectedTrades = 1 + (int)MathFloor(priceDiffPips / DistancePips);
    
    Print("Current price: ", currentPrice, " First trade price: ", firstTradePrice);
    Print("Price diff pips: ", priceDiffPips, " Expected trades: ", expectedTrades, " Current trade count: ", tradeCount);
    
    //--- Open additional trades if we've moved enough distance down
    if(expectedTrades > tradeCount)
    {
        int tradesToOpen = expectedTrades - tradeCount;
        
        for(int i = 0; i < tradesToOpen && tradeCount < MaxTrades; i++)
        {
            OpenBuyPosition(lotSizes[tradeCount]);
            tradeCount++;
            Print("Martingale trade opened. Trade count: ", tradeCount);
        }
        
        UpdateAveragePrice();
    }
}

//+------------------------------------------------------------------+
//| Open buy position                                                |
//+------------------------------------------------------------------+
void OpenBuyPosition(double lotSize)
{
    MqlTradeRequest request;
    MqlTradeResult result;
    ZeroMemory(request);
    ZeroMemory(result);
    
    double price = SymbolInfoDouble(_Symbol, SYMBOL_ASK);
    
    request.action = TRADE_ACTION_DEAL;
    request.symbol = _Symbol;
    request.volume = lotSize;
    request.type = ORDER_TYPE_BUY;
    request.price = price;
    request.sl = 0; // No stop loss
    request.tp = 0; // TP will be managed separately for all trades
    request.magic = MagicNumber;
    request.comment = "Martingale Trade " + IntegerToString(tradeCount + 1);
    
    //--- Send order
    if(OrderSend(request, result))
    {
        Print("Buy order opened. Ticket: ", result.order, " Lot size: ", lotSize, " Price: ", price);
        UpdateAveragePrice();
    }
    else
    {
        Print("Error opening buy order: ", GetLastError());
    }
}

//+------------------------------------------------------------------+
//| Update average price of all positions                            |
//+------------------------------------------------------------------+
void UpdateAveragePrice()
{
    totalVolume = 0;
    averagePrice = 0;
    
    for(int i = 0; i < PositionsTotal(); i++)
    {
        if(PositionGetSymbol(i) == _Symbol && PositionGetInteger(POSITION_MAGIC) == MagicNumber)
        {
            double volume = PositionGetDouble(POSITION_VOLUME);
            double price = PositionGetDouble(POSITION_PRICE_OPEN);
            
            totalVolume += volume;
            averagePrice += price * volume;
        }
    }
    
    if(totalVolume > 0)
        averagePrice /= totalVolume;
        
    Print("Average price updated: ", averagePrice, " Total volume: ", totalVolume);
}

//+------------------------------------------------------------------+
//| Check if total profit target is reached                          |
//+------------------------------------------------------------------+
void CheckForTP()
{
    if(CountPositions() == 0) return;
    
    double currentPrice = SymbolInfoDouble(_Symbol, SYMBOL_BID);
    double profitPoints = (currentPrice - averagePrice) / 
                         SymbolInfoDouble(_Symbol, SYMBOL_POINT);
    double profitPips = profitPoints / 10;
    
    if(profitPips >= TakeProfitPips)
    {
        Print("Take profit reached! Profit: ", profitPips, " pips. Average price: ", averagePrice, " Current price: ", currentPrice);
        CloseAllPositions();
    }
}

//+------------------------------------------------------------------+
//| Close all positions                                              |
//+------------------------------------------------------------------+
void CloseAllPositions()
{
    int closedCount = 0;
    double totalProfit = 0;
    
    for(int i = PositionsTotal() - 1; i >= 0; i--)
    {
        if(PositionGetSymbol(i) == _Symbol && PositionGetInteger(POSITION_MAGIC) == MagicNumber)
        {
            MqlTradeRequest request;
            MqlTradeResult result;
            ZeroMemory(request);
            ZeroMemory(result);
            
            ulong ticket = PositionGetInteger(POSITION_TICKET);
            double volume = PositionGetDouble(POSITION_VOLUME);
            double profit = PositionGetDouble(POSITION_PROFIT);
            totalProfit += profit;
            
            request.action = TRADE_ACTION_DEAL;
            request.symbol = _Symbol;
            request.volume = volume;
            request.type = ORDER_TYPE_SELL;
            request.price = SymbolInfoDouble(_Symbol, SYMBOL_BID);
            request.position = ticket;
            request.magic = MagicNumber;
            
            if(OrderSend(request, result))
            {
                Print("Position closed. Ticket: ", ticket, " Profit: ", profit);
                closedCount++;
            }
            else
            {
                Print("Error closing position: ", GetLastError());
            }
        }
    }
    
    if(closedCount > 0)
    {
        Print("All ", closedCount, " positions closed. Total profit: ", totalProfit);
        tradeCount = 0;
        totalVolume = 0;
        averagePrice = 0;
        firstTradePrice = 0;
    }
}

//+------------------------------------------------------------------+
//| Count positions for this EA                                      |
//+------------------------------------------------------------------+
int CountPositions()
{
    int count = 0;
    for(int i = 0; i < PositionsTotal(); i++)
    {
        if(PositionGetSymbol(i) == _Symbol && PositionGetInteger(POSITION_MAGIC) == MagicNumber)
        {
            count++;
        }
    }
    return count;
}

//+------------------------------------------------------------------+
//| Get current ATR value                                            |
//+------------------------------------------------------------------+
double GetATRValue()
{
    double atr[1];
    if(CopyBuffer(atrHandle, 0, 0, 1, atr) > 0)
    {
        return NormalizeDouble(atr[0], 2);
    }
    return 100; // Return high value if error
}
//+------------------------------------------------------------------+
