#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "backtest.h"
#include "structures.h"

double calculate_sma(double prices[], int current_day, int period) {
    if (current_day < period - 1) return 0.0;
    
    double sum = 0.0;
    for (int i = 0; i < period; i++) {
        sum += prices[current_day - i];
    }
    return sum / period;
}

double calculate_rsi(double prices[], int current_day, int period) {
    if (current_day < period) return 50.0;

    double gains = 0.0, losses = 0.0;
    
    for (int i = 1; i <= period; i++) {
        double change = prices[current_day - period + i] - prices[current_day - period + i - 1];
        if (change > 0) gains += change;
        else losses += -change;
    }

    double avg_gain = gains / period;
    double avg_loss = losses / period;

    if (avg_loss == 0.0) return 100.0;
    
    double rs = avg_gain / avg_loss;
    return 100.0 - (100.0 / (1.0 + rs));
}

void backtest(Stock stocks[], int stock_count, Strategy strategy, Portfolio *portfolio) {
    int max_days = stocks[0].day_count;

    for (int day = 20; day < max_days; day++) {
        for (int s = 0; s < stock_count; s++) {
            double current_price = stocks[s].prices[day].close;
            
            double close_prices[MAX_DAYS];
            for (int i = 0; i < day + 1; i++) {
                close_prices[i] = stocks[s].prices[i].close;
            }

            if (portfolio->positions[s] > 0) {
                double buy_price = portfolio->avg_buy_price[s];
                double profit_pct = ((current_price - buy_price) / buy_price) * 100.0;
                int holding_days = day - portfolio->buy_day[s];

                int should_sell = 0;
                char reason[100] = "";

                if (profit_pct >= strategy.take_profit_pct) {
                    should_sell = 1;
                    sprintf(reason, "Take Profit (%.2f%% gain)", profit_pct);
                } else if (profit_pct <= -strategy.stop_loss_pct) {
                    should_sell = 1;
                    sprintf(reason, "Stop Loss (%.2f%% loss)", profit_pct);
                } else if (holding_days >= strategy.max_holding_days) {
                    should_sell = 1;
                    sprintf(reason, "Max Holding Period (%d days)", holding_days);
                } else if (strategy.rsi_overbought < 100) {
                    double rsi = calculate_rsi(close_prices, day, 14);
                    if (rsi >= strategy.rsi_overbought) {
                        should_sell = 1;
                        sprintf(reason, "RSI Overbought (RSI: %.2f)", rsi);
                    }
                }

                if (should_sell) {
                    int quantity = portfolio->positions[s];
                    double total_value = current_price * quantity;
                    double profit = (current_price - buy_price) * quantity;
                    
                    Trade *trade = &portfolio->trades[portfolio->trade_count++];
                    strcpy(trade->symbol, stocks[s].symbol);
                    strcpy(trade->date, stocks[s].prices[day].date);
                    trade->day = day;
                    strcpy(trade->type, "SELL");
                    trade->price = current_price;
                    trade->quantity = quantity;
                    trade->total_value = total_value;
                    trade->portfolio_cash_before = portfolio->cash;
                    portfolio->cash += total_value;
                    trade->portfolio_cash_after = portfolio->cash;
                    trade->profit_loss = profit;
                    strcpy(trade->reason, reason);

                    portfolio->positions[s] = 0;
                    portfolio->avg_buy_price[s] = 0.0;
                    portfolio->buy_day[s] = -1;
                }
            } else {
                int should_buy = 0;
                char reason[100] = "";

                if (strategy.sma_short_period > 0 && strategy.sma_long_period > 0) {
                    double sma_short = calculate_sma(close_prices, day, strategy.sma_short_period);
                    double sma_long = calculate_sma(close_prices, day, strategy.sma_long_period);
                    double prev_sma_short = calculate_sma(close_prices, day - 1, strategy.sma_short_period);
                    double prev_sma_long = calculate_sma(close_prices, day - 1, strategy.sma_long_period);

                    if (prev_sma_short <= prev_sma_long && sma_short > sma_long) {
                        should_buy = 1;
                        sprintf(reason, "SMA Crossover (Short:%.2f > Long:%.2f)", sma_short, sma_long);
                    }
                }

                if (!should_buy && strategy.rsi_oversold > 0) {
                    double rsi = calculate_rsi(close_prices, day, 14);
                    if (rsi <= strategy.rsi_oversold) {
                        should_buy = 1;
                        sprintf(reason, "RSI Oversold (RSI: %.2f)", rsi);
                    }
                }

                if (should_buy) {
                    double investment = portfolio->cash * 0.2;
                    int quantity = (int)(investment / current_price);

                    if (quantity > 0 && portfolio->cash >= current_price * quantity) {
                        double total_value = current_price * quantity;
                        
                        Trade *trade = &portfolio->trades[portfolio->trade_count++];
                        strcpy(trade->symbol, stocks[s].symbol);
                        strcpy(trade->date, stocks[s].prices[day].date);
                        trade->day = day;
                        strcpy(trade->type, "BUY");
                        trade->price = current_price;
                        trade->quantity = quantity;
                        trade->total_value = total_value;
                        trade->portfolio_cash_before = portfolio->cash;
                        portfolio->cash -= total_value;
                        trade->portfolio_cash_after = portfolio->cash;
                        trade->profit_loss = 0.0;
                        strcpy(trade->reason, reason);

                        portfolio->positions[s] = quantity;
                        portfolio->avg_buy_price[s] = current_price;
                        portfolio->buy_day[s] = day;
                    }
                }
            }
        }
    }
}

void print_detailed_results(Portfolio *portfolio, Stock stocks[], int stock_count, double initial_cash) {
    printf("\n\n");
    printf("================================================================================\n");
    printf("                          DETAILED BACKTEST RESULTS                             \n");
    printf("================================================================================\n\n");

    printf("COMPLETE TRADE HISTORY:\n");
    printf("================================================================================\n\n");

    for (int i = 0; i < portfolio->trade_count; i++) {
        Trade *t = &portfolio->trades[i];
        
        printf("TRADE #%d - %s %s\n", i + 1, t->type, t->symbol);
        printf("--------------------------------------------------------------------------------\n");
        printf("Date:                    %s (Day %d)\n", t->date, t->day);
        printf("Reason:                  %s\n", t->reason);
        printf("Price per Share:         $%.2f\n", t->price);
        printf("Quantity:                %d shares\n", t->quantity);
        printf("Total Transaction Value: $%.2f\n", t->total_value);
        printf("Portfolio Cash Before:   $%.2f\n", t->portfolio_cash_before);
        printf("Portfolio Cash After:    $%.2f\n", t->portfolio_cash_after);
        
        if (strcmp(t->type, "SELL") == 0) {
            if (t->profit_loss >= 0) {
                printf("Profit:                  $%.2f ✓\n", t->profit_loss);
            } else {
                printf("Loss:                    $%.2f ✗\n", t->profit_loss);
            }
        }
        printf("\n");
    }

    double portfolio_value = portfolio->cash;
    int buy_count = 0, sell_count = 0;
    double total_realized_profit = 0.0;
    int winning_trades = 0, losing_trades = 0;
    double total_invested = 0.0;

    for (int i = 0; i < portfolio->trade_count; i++) {
        Trade *t = &portfolio->trades[i];
        if (strcmp(t->type, "BUY") == 0) {
            buy_count++;
            total_invested += t->total_value;
        } else {
            sell_count++;
            total_realized_profit += t->profit_loss;
            if (t->profit_loss > 0) winning_trades++;
            else losing_trades++;
        }
    }

    for (int i = 0; i < stock_count; i++) {
        if (portfolio->positions[i] > 0) {
            int last_day = stocks[i].day_count - 1;
            portfolio_value += portfolio->positions[i] * stocks[i].prices[last_day].close;
        }
    }

    double total_return = portfolio_value - initial_cash;
    double return_pct = (total_return / initial_cash) * 100.0;

    printf("================================================================================\n");
    printf("                              PORTFOLIO SUMMARY                                 \n");
    printf("================================================================================\n\n");
    printf("Initial Capital:         $%.2f\n", initial_cash);
    printf("Final Portfolio Value:   $%.2f\n", portfolio_value);
    printf("Final Cash Balance:      $%.2f\n", portfolio->cash);
    printf("Total Return:            $%.2f (%.2f%%)\n", total_return, return_pct);
    printf("Total Realized Profit:   $%.2f\n", total_realized_profit);
    printf("\n");

    printf("TRADING STATISTICS:\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("Total Trades:            %d\n", portfolio->trade_count);
    printf("Buy Orders:              %d\n", buy_count);
    printf("Sell Orders:             %d\n", sell_count);
    printf("Winning Trades:          %d\n", winning_trades);
    printf("Losing Trades:           %d\n", losing_trades);
    if (sell_count > 0) {
        printf("Win Rate:                %.2f%%\n", (double)winning_trades / sell_count * 100.0);
        printf("Average Profit per Trade: $%.2f\n", total_realized_profit / sell_count);
    }
    printf("Total Money Invested:    $%.2f\n", total_invested);
    printf("\n");

    printf("CURRENT OPEN POSITIONS:\n");
    printf("--------------------------------------------------------------------------------\n");
    int has_positions = 0;
    for (int i = 0; i < stock_count; i++) {
        if (portfolio->positions[i] > 0) {
            has_positions = 1;
            int last_day = stocks[i].day_count - 1;
            double current_price = stocks[i].prices[last_day].close;
            double current_value = portfolio->positions[i] * current_price;
            double unrealized = (current_price - portfolio->avg_buy_price[i]) * portfolio->positions[i];
            double unrealized_pct = (unrealized / (portfolio->avg_buy_price[i] * portfolio->positions[i])) * 100.0;
            
            printf("%s:\n", stocks[i].symbol);
            printf("  Quantity:              %d shares\n", portfolio->positions[i]);
            printf("  Average Buy Price:     $%.2f\n", portfolio->avg_buy_price[i]);
            printf("  Current Price:         $%.2f\n", current_price);
            printf("  Position Value:        $%.2f\n", current_value);
            printf("  Unrealized P/L:        $%.2f (%.2f%%)\n\n", unrealized, unrealized_pct);
        }
    }
    if (!has_positions) {
        printf("No open positions - All cash\n\n");
    }

    printf("================================================================================\n");
}

void calculate_strategy_result(Portfolio *portfolio, Stock stocks[], int stock_count, 
                               double initial_cash, Strategy strategy, 
                               StrategyResult *result, char *username) {
    strcpy(result->strategy_name, strategy.name);
    strcpy(result->username, username);
    result->initial_capital = initial_cash;
    
    double portfolio_value = portfolio->cash;
    int winning = 0, losing = 0;
    double realized_profit = 0.0;
    
    for (int i = 0; i < stock_count; i++) {
        if (portfolio->positions[i] > 0) {
            int last_day = stocks[i].day_count - 1;
            portfolio_value += portfolio->positions[i] * stocks[i].prices[last_day].close;
        }
    }
    
    for (int i = 0; i < portfolio->trade_count; i++) {
        if (strcmp(portfolio->trades[i].type, "SELL") == 0) {
            realized_profit += portfolio->trades[i].profit_loss;
            if (portfolio->trades[i].profit_loss > 0) winning++;
            else losing++;
        }
    }
    
    result->final_value = portfolio_value;
    result->total_return = portfolio_value - initial_cash;
    result->return_pct = (result->total_return / initial_cash) * 100.0;
    result->total_trades = portfolio->trade_count;
    result->winning_trades = winning;
    result->losing_trades = losing;
    result->win_rate = (winning + losing > 0) ? (double)winning / (winning + losing) * 100.0 : 0.0;
    result->total_realized_profit = realized_profit;
}

void compare_strategies(StrategyResult results[], int result_count) {
    printf("\n\n");
    printf("================================================================================\n");
    printf("                        STRATEGY COMPARISON REPORT                              \n");
    printf("================================================================================\n\n");
    
    int best_idx = 0;
    for (int i = 1; i < result_count; i++) {
        if (results[i].return_pct > results[best_idx].return_pct) {
            best_idx = i;
        }
    }
    
    printf("PERFORMANCE COMPARISON:\n");
    printf("================================================================================\n\n");
    
    for (int i = 0; i < result_count; i++) {
        StrategyResult *r = &results[i];
        printf("STRATEGY #%d: %s", i + 1, r->strategy_name);
        if (i == best_idx) printf(" ⭐ BEST PERFORMER");
        printf("\n");
        printf("--------------------------------------------------------------------------------\n");
        printf("User:                    %s\n", r->username);
        printf("Initial Capital:         $%.2f\n", r->initial_capital);
        printf("Final Value:             $%.2f\n", r->final_value);
        printf("Total Return:            $%.2f (%.2f%%)\n", r->total_return, r->return_pct);
        printf("Total Trades:            %d\n", r->total_trades);
        printf("Winning Trades:          %d\n", r->winning_trades);
        printf("Losing Trades:           %d\n", r->losing_trades);
        printf("Win Rate:                %.2f%%\n", r->win_rate);
        printf("Realized Profit:         $%.2f\n", r->total_realized_profit);
        printf("\n");
    }
    
    printf("================================================================================\n");
    printf("                            RANKING BY RETURN %%                                \n");
    printf("================================================================================\n\n");
    
    StrategyResult sorted[10];
    memcpy(sorted, results, result_count * sizeof(StrategyResult));
    
    for (int i = 0; i < result_count - 1; i++) {
        for (int j = 0; j < result_count - i - 1; j++) {
            if (sorted[j].return_pct < sorted[j + 1].return_pct) {
                StrategyResult temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }
    
    printf("Rank | Strategy Name                    | Return %%   | Total Return\n");
    printf("--------------------------------------------------------------------------------\n");
    for (int i = 0; i < result_count; i++) {
        printf("%-4d | %-32s | %8.2f%% | $%.2f\n", 
               i + 1, sorted[i].strategy_name, sorted[i].return_pct, sorted[i].total_return);
    }
    printf("\n");
}

void get_preset_strategy(Strategy *strategy) {
    int choice;
    
    printf("\nSelect Preset Trading Strategy:\n");
    printf("1. Simple Moving Average Crossover\n");
    printf("2. RSI-based Strategy\n");
    printf("3. Combined Strategy (SMA + RSI)\n");
    printf("Enter choice (1-3): ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            strcpy(strategy->name, "SMA Crossover");
            strategy->sma_short_period = 5;
            strategy->sma_long_period = 20;
            strategy->rsi_oversold = 0;
            strategy->rsi_overbought = 100;
            strategy->stop_loss_pct = 5.0;
            strategy->take_profit_pct = 10.0;
            strategy->max_holding_days = 15;
            break;
        case 2:
            strcpy(strategy->name, "RSI Strategy");
            strategy->rsi_oversold = 30;
            strategy->rsi_overbought = 70;
            strategy->sma_short_period = 0;
            strategy->sma_long_period = 0;
            strategy->stop_loss_pct = 4.0;
            strategy->take_profit_pct = 8.0;
            strategy->max_holding_days = 10;
            break;
        case 3:
            strcpy(strategy->name, "Combined Strategy");
            strategy->sma_short_period = 5;
            strategy->sma_long_period = 20;
            strategy->rsi_oversold = 30;
            strategy->rsi_overbought = 70;
            strategy->stop_loss_pct = 5.0;
            strategy->take_profit_pct = 12.0;
            strategy->max_holding_days = 20;
            break;
        default:
            strcpy(strategy->name, "Combined Strategy");
            strategy->sma_short_period = 5;
            strategy->sma_long_period = 20;
            strategy->rsi_oversold = 30;
            strategy->rsi_overbought = 70;
            strategy->stop_loss_pct = 5.0;
            strategy->take_profit_pct = 12.0;
            strategy->max_holding_days = 20;
    }

    printf("\n=== STRATEGY CONFIGURED ===\n");
    printf("Name: %s\n", strategy->name);
    if (strategy->sma_short_period > 0) {
        printf("SMA Short Period: %d days\n", strategy->sma_short_period);
        printf("SMA Long Period: %d days\n", strategy->sma_long_period);
    }
    if (strategy->rsi_oversold > 0) {
        printf("RSI Oversold: %.0f\n", strategy->rsi_oversold);
        printf("RSI Overbought: %.0f\n", strategy->rsi_overbought);
    }
    printf("Stop Loss: %.2f%%\n", strategy->stop_loss_pct);
    printf("Take Profit: %.2f%%\n", strategy->take_profit_pct);
    printf("Max Holding: %d days\n", strategy->max_holding_days);
}

void run_comparison_backtest(Stock stocks[], int stock_count, User *user, double initial_cash) {
    printf("\n=== RUNNING COMPARISON BACKTEST ===\n");
    printf("Testing all strategies against the same stock data...\n\n");
    
    StrategyResult results[10];
    int result_count = 0;
    
    Strategy preset_strategies[3];
    
    strcpy(preset_strategies[0].name, "SMA Crossover");
    preset_strategies[0].sma_short_period = 5;
    preset_strategies[0].sma_long_period = 20;
    preset_strategies[0].rsi_oversold = 0;
    preset_strategies[0].rsi_overbought = 100;
    preset_strategies[0].stop_loss_pct = 5.0;
    preset_strategies[0].take_profit_pct = 10.0;
    preset_strategies[0].max_holding_days = 15;
    
    strcpy(preset_strategies[1].name, "RSI Strategy");
    preset_strategies[1].rsi_oversold = 30;
    preset_strategies[1].rsi_overbought = 70;
    preset_strategies[1].sma_short_period = 0;
    preset_strategies[1].sma_long_period = 0;
    preset_strategies[1].stop_loss_pct = 4.0;
    preset_strategies[1].take_profit_pct = 8.0;
    preset_strategies[1].max_holding_days = 10;
    
    strcpy(preset_strategies[2].name, "Combined Strategy");
    preset_strategies[2].sma_short_period = 5;
    preset_strategies[2].sma_long_period = 20;
    preset_strategies[2].rsi_oversold = 30;
    preset_strategies[2].rsi_overbought = 70;
    preset_strategies[2].stop_loss_pct = 5.0;
    preset_strategies[2].take_profit_pct = 12.0;
    preset_strategies[2].max_holding_days = 20;
    
    for (int i = 0; i < 3; i++) {
        Portfolio portfolio;
        portfolio.cash = initial_cash;
        portfolio.trade_count = 0;
        for (int j = 0; j < MAX_STOCKS; j++) {
            portfolio.positions[j] = 0;
            portfolio.avg_buy_price[j] = 0.0;
            portfolio.buy_day[j] = -1;
        }
        
        backtest(stocks, stock_count, preset_strategies[i], &portfolio);
        calculate_strategy_result(&portfolio, stocks, stock_count, initial_cash, 
                                 preset_strategies[i], &results[result_count], "System");
        result_count++;
        printf("Completed: %s\n", preset_strategies[i].name);
    }
    
    for (int i = 0; i < user->strategy_count; i++) {
        Portfolio portfolio;
        portfolio.cash = initial_cash;
        portfolio.trade_count = 0;
        for (int j = 0; j < MAX_STOCKS; j++) {
            portfolio.positions[j] = 0;
            portfolio.avg_buy_price[j] = 0.0;
            portfolio.buy_day[j] = -1;
        }
        
        backtest(stocks, stock_count, user->custom_strategies[i], &portfolio);
        calculate_strategy_result(&portfolio, stocks, stock_count, initial_cash, 
                                 user->custom_strategies[i], &results[result_count], user->username);
        result_count++;
        printf("Completed: %s (User: %s)\n", user->custom_strategies[i].name, user->username);
    }
    
    compare_strategies(results, result_count);
}