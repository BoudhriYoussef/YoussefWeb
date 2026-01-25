//+--------Youssef BOUDHRI protect-------------------------------------+
//|                                                  ATRReversalEA.mq5 |
//|                        Copyright 2023, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2023, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property version   "1.00"

//+------------------------------------------------------------------+
//| Input parameters                                                 |
//+------------------------------------------------------------------+
input int      ATR_Period = 14;               // Période ATR
input double   ATR_Multiplier = 2.0;          // Multiplicateur ATR
input double   LotSize = 0.1;                 // Taille du lot
input int      StopLoss_Pips = 30;            // Stop Loss (pips)
input int      TakeProfit_Pips = 50;          // Take Profit (pips)
input int      MagicNumber = 12345;           // Magic Number
input int      Slippage = 3;                  // Slippage
input bool     UseTrailingStop = true;        // Utiliser Trailing Stop
input int      TrailingStop_Pips = 20;        // Trailing Stop (pips)

//+------------------------------------------------------------------+
//| Global variables                                                 |
//+------------------------------------------------------------------+
int atrHandle;
double buyStopPrice, sellStopPrice;
ulong buyTicket, sellTicket;
double pointMultiplier; // Multiplicateur pour gérer les pips

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
{
   // Créer le handle pour l'indicateur ATR
   atrHandle = iATR(_Symbol, _Period, ATR_Period);
   if(atrHandle == INVALID_HANDLE)
   {
      Print("Erreur lors de la création de l'ATR");
      return(INIT_FAILED);
   }
   
   // Calculer le multiplicateur de points pour les pips
   pointMultiplier = CalculatePointMultiplier();
   
   // Supprimer les ordres en attente existants
   DeletePendingOrders();
   
   return(INIT_SUCCEEDED);
}

//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
{
   // Supprimer le handle de l'indicateur
   if(atrHandle != INVALID_HANDLE)
      IndicatorRelease(atrHandle);
}

//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
{
   // Vérifier si nous avons assez de bars
   if(Bars(_Symbol, _Period) < ATR_Period + 10)
      return;
   
   // Calculer les prix des ordres stop
   CalculateStopPrices();
   
   // Gérer les ordres en attente
   ManagePendingOrders();
   
   // Gérer le trailing stop
   if(UseTrailingStop)
      ManageTrailingStop();
}

//+------------------------------------------------------------------+
//| Calculer le multiplicateur de points                            |
//+------------------------------------------------------------------+
double CalculatePointMultiplier()
{
   // Pour les paires Forex (4-5 digits)
   if(_Digits == 4 || _Digits == 5)
      return 10.0;
   // Pour les indices, commodities, etc.
   else
      return 1.0;
}

//+------------------------------------------------------------------+
//| Convertir les pips en prix                                      |
//+------------------------------------------------------------------+
double PipsToPrice(int pips)
{
   return pips * _Point * pointMultiplier;
}

//+------------------------------------------------------------------+
//| Calculer les prix des ordres stop                               |
//+------------------------------------------------------------------+
void CalculateStopPrices()
{
   double atrValue[1];
   
   // Récupérer la valeur ATR
   if(CopyBuffer(atrHandle, 0, 0, 1, atrValue) < 1)
   {
      Print("Erreur lors de la copie des données ATR");
      return;
   }
   
   double currentHigh = iHigh(_Symbol, _Period, 1);
   double currentLow = iLow(_Symbol, _Period, 1);
   
   // Ordres inversés comme demandé
   // Buy Stop: En-dessous du bas (cassure de support)
   // Sell Stop: Au-dessus du haut (cassure de résistance)
   buyStopPrice = currentLow - (atrValue[0] * ATR_Multiplier);
   sellStopPrice = currentHigh + (atrValue[0] * ATR_Multiplier);
   
   // Ajuster aux digits du symbole
   buyStopPrice = NormalizeDouble(buyStopPrice, _Digits);
   sellStopPrice = NormalizeDouble(sellStopPrice, _Digits);
   
   Print("Buy Stop (Support): ", buyStopPrice, " | Sell Stop (Résistance): ", sellStopPrice, " | ATR: ", atrValue[0]);
}

//+------------------------------------------------------------------+
//| Gérer les ordres en attente                                     |
//+------------------------------------------------------------------+
void ManagePendingOrders()
{
   bool buyOrderExists = false;
   bool sellOrderExists = false;
   
   // Vérifier les positions existantes
   for(int i = 0; i < PositionsTotal(); i++)
   {
      ulong ticket = PositionGetTicket(i);
      if(PositionGetInteger(POSITION_MAGIC) == MagicNumber)
      {
         if(PositionGetInteger(POSITION_TYPE) == POSITION_TYPE_BUY)
            buyOrderExists = true;
         else if(PositionGetInteger(POSITION_TYPE) == POSITION_TYPE_SELL)
            sellOrderExists = true;
      }
   }
   
   // Vérifier les ordres en attente
   for(int i = 0; i < OrdersTotal(); i++)
   {
      if(OrderGetTicket(i))
      {
         if(OrderGetInteger(ORDER_MAGIC) == MagicNumber)
         {
            if(OrderGetInteger(ORDER_TYPE) == ORDER_TYPE_BUY_STOP)
               buyOrderExists = true;
            else if(OrderGetInteger(ORDER_TYPE) == ORDER_TYPE_SELL_STOP)
               sellOrderExists = true;
         }
      }
   }
   
   // Placer l'ordre Buy Stop s'il n'existe pas (Cassure de support)
   if(!buyOrderExists)
   {
      // Calculer SL et TP en pips
      double sl = buyStopPrice - PipsToPrice(StopLoss_Pips);
      double tp = buyStopPrice + PipsToPrice(TakeProfit_Pips);
      
      MqlTradeRequest request;
      MqlTradeResult result;
      ZeroMemory(request);
      ZeroMemory(result);
      
      request.action = TRADE_ACTION_PENDING;
      request.symbol = _Symbol;
      request.volume = LotSize;
      request.type = ORDER_TYPE_BUY_STOP;
      request.price = NormalizeDouble(buyStopPrice, _Digits);
      request.sl = NormalizeDouble(sl, _Digits);
      request.tp = NormalizeDouble(tp, _Digits);
      request.magic = MagicNumber;
      request.deviation = Slippage;
      
      if(OrderSend(request, result))
         Print("BUY STOP - Support: ", DoubleToString(buyStopPrice, _Digits), 
               " | SL: ", DoubleToString(sl, _Digits), 
               " | TP: ", DoubleToString(tp, _Digits));
      else
         Print("Erreur Buy Stop: ", GetLastError());
   }
   
   // Placer l'ordre Sell Stop s'il n'existe pas (Cassure de résistance)
   if(!sellOrderExists)
   {
      // Calculer SL et TP en pips
      double sl = sellStopPrice + PipsToPrice(StopLoss_Pips);
      double tp = sellStopPrice - PipsToPrice(TakeProfit_Pips);
      
      MqlTradeRequest request;
      MqlTradeResult result;
      ZeroMemory(request);
      ZeroMemory(result);
      
      request.action = TRADE_ACTION_PENDING;
      request.symbol = _Symbol;
      request.volume = LotSize;
      request.type = ORDER_TYPE_SELL_STOP;
      request.price = NormalizeDouble(sellStopPrice, _Digits);
      request.sl = NormalizeDouble(sl, _Digits);
      request.tp = NormalizeDouble(tp, _Digits);
      request.magic = MagicNumber;
      request.deviation = Slippage;
      
      if(OrderSend(request, result))
         Print("SELL STOP - Résistance: ", DoubleToString(sellStopPrice, _Digits),
               " | SL: ", DoubleToString(sl, _Digits),
               " | TP: ", DoubleToString(tp, _Digits));
      else
         Print("Erreur Sell Stop: ", GetLastError());
   }
}

//+------------------------------------------------------------------+
//| Gérer le trailing stop                                          |
//+------------------------------------------------------------------+
void ManageTrailingStop()
{
   for(int i = 0; i < PositionsTotal(); i++)
   {
      ulong ticket = PositionGetTicket(i);
      if(PositionGetInteger(POSITION_MAGIC) == MagicNumber)
      {
         double newSl = 0;
         double currentSl = PositionGetDouble(POSITION_SL);
         
         if(PositionGetInteger(POSITION_TYPE) == POSITION_TYPE_BUY)
         {
            newSl = SymbolInfoDouble(_Symbol, SYMBOL_BID) - PipsToPrice(TrailingStop_Pips);
            newSl = NormalizeDouble(newSl, _Digits);
            
            if(newSl > currentSl && newSl < SymbolInfoDouble(_Symbol, SYMBOL_BID))
            {
               ModifyStopLoss(ticket, newSl);
            }
         }
         else if(PositionGetInteger(POSITION_TYPE) == POSITION_TYPE_SELL)
         {
            newSl = SymbolInfoDouble(_Symbol, SYMBOL_ASK) + PipsToPrice(TrailingStop_Pips);
            newSl = NormalizeDouble(newSl, _Digits);
            
            if((newSl < currentSl || currentSl == 0) && newSl > SymbolInfoDouble(_Symbol, SYMBOL_ASK))
            {
               ModifyStopLoss(ticket, newSl);
            }
         }
      }
   }
}

//+------------------------------------------------------------------+
//| Modifier le stop loss                                           |
//+------------------------------------------------------------------+
bool ModifyStopLoss(ulong ticket, double newSl)
{
   MqlTradeRequest request;
   MqlTradeResult result;
   ZeroMemory(request);
   ZeroMemory(result);
   
   request.action = TRADE_ACTION_SLTP;
   request.position = ticket;
   request.symbol = _Symbol;
   request.sl = newSl;
   request.magic = MagicNumber;
   
   if(OrderSend(request, result))
   {
      Print("Stop Loss modifié pour le ticket: ", ticket, " -> New SL: ", newSl);
      return true;
   }
   else
   {
      Print("Erreur modification SL: ", GetLastError());
      return false;
   }
}

//+------------------------------------------------------------------+
//| Supprimer les ordres en attente                                 |
//+------------------------------------------------------------------+
void DeletePendingOrders()
{
   for(int i = OrdersTotal() - 1; i >= 0; i--)
   {
      if(OrderGetTicket(i))
      {
         if(OrderGetInteger(ORDER_MAGIC) == MagicNumber)
         {
            MqlTradeRequest request;
            MqlTradeResult result;
            ZeroMemory(request);
            ZeroMemory(result);
            
            request.action = TRADE_ACTION_REMOVE;
            request.order = OrderGetTicket(i);
            
            OrderSend(request, result);
         }
      }
   }
}
//+------------------------------------------------------------------+
