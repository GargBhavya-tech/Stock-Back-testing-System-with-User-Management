#ifndef STRUCTURES_H
#define STRUCTURES_H

#define MAX_STOCKS 10
#define MAX_DAYS 1000
#define MAX_TRADES 500
#define MAX_STOCK_NAME 20
#define MAX_USERS 50
#define MAX_USERNAME 30
#define MAX_PASSWORD 30
#define MAX_STRATEGIES_PER_USER 10

// Stock price data structure
typedef struct {
    char symbol[MAX_STOCK_NAME];
    char date[12];
    double open;
    double high;
    double low;
    double close;
    int volume;
} PriceData;

// Stock structure
typedef struct {
    char symbol[MAX_STOCK_NAME];
    PriceData prices[MAX_DAYS];
    int day_count;
} Stock;

// Trade record structure
typedef struct {
    char symbol[MAX_STOCK_NAME];
    char date[12];
    int day;
    char type[5];
    double price;
    int quantity;
    double total_value;
    double portfolio_cash_before;
    double portfolio_cash_after;
    double profit_loss;
    char reason[100];
} Trade;

// Strategy structure
typedef struct {
    char name[50];
    double rsi_oversold;
    double rsi_overbought;
    int sma_short_period;
    int sma_long_period;
    double stop_loss_pct;
    double take_profit_pct;
    int max_holding_days;
} Strategy;

// Portfolio structure
typedef struct {
    double cash;
    int positions[MAX_STOCKS];
    double avg_buy_price[MAX_STOCKS];
    int buy_day[MAX_STOCKS];
    Trade trades[MAX_TRADES];
    int trade_count;
} Portfolio;

// User structure
typedef struct {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    Strategy custom_strategies[MAX_STRATEGIES_PER_USER];
    int strategy_count;
} User;

// Strategy Result for comparison
typedef struct {
    char strategy_name[50];
    char username[MAX_USERNAME];
    double initial_capital;
    double final_value;
    double total_return;
    double return_pct;
    int total_trades;
    int winning_trades;
    int losing_trades;
    double win_rate;
    double total_realized_profit;
} StrategyResult;

#endif