#ifndef BACKTEST_H
#define BACKTEST_H

#include "structures.h"

double calculate_sma(double prices[], int current_day, int period);
double calculate_rsi(double prices[], int current_day, int period);
void backtest(Stock stocks[], int stock_count, Strategy strategy, Portfolio *portfolio);
void print_detailed_results(Portfolio *portfolio, Stock stocks[], int stock_count, double initial_cash);
void calculate_strategy_result(Portfolio *portfolio, Stock stocks[], int stock_count, 
                               double initial_cash, Strategy strategy, 
                               StrategyResult *result, char *username);
void compare_strategies(StrategyResult results[], int result_count);
void run_comparison_backtest(Stock stocks[], int stock_count, User *user, double initial_cash);
void get_preset_strategy(Strategy *strategy);

#endif