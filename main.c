#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "user_management.h"
#include "stock_data.h"
#include "backtest.h"

int main() {
    Stock stocks[MAX_STOCKS];
    int stock_count = 0;
    User users[MAX_USERS];
    int user_count = 0;
    char logged_username[MAX_USERNAME];
    User *current_user = NULL;

    printf("╔════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║            STOCK BACKTESTING SYSTEM WITH USER LOGIN                       ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════╝\n\n");

    // Load users
    load_users(users, &user_count);
    printf("Loaded %d existing users from database.\n\n", user_count);

    // Login/Register
    int choice;
    printf("╔═══════════════════════════════════╗\n");
    printf("║         AUTHENTICATION            ║\n");
    printf("╚═══════════════════════════════════╝\n");
    printf("1. Login\n");
    printf("2. Register New Account\n");
    printf("3. Exit\n");
    printf("\nEnter choice: ");
    scanf("%d", &choice);

    if (choice == 3) {
        printf("\nThank you for using the Stock Backtesting System!\n");
        return 0;
    }

    if (choice == 2) {
        register_user(users, &user_count);
        save_users(users, user_count);
        printf("\nRegistration successful! Please login.\n\n");
    }

    int user_index = login(users, user_count, logged_username);
    if (user_index == -1) {
        printf("\n❌ Login failed! Invalid username or password.\n");
        printf("Exiting...\n");
        return 1;
    }
    current_user = &users[user_index];
    printf("\n✓ Welcome, %s!\n", logged_username);
    printf("You have %d saved strategies.\n\n", current_user->strategy_count);

    // Create sample CSV file
    printf("Preparing stock data...\n");
    create_sample_csv();
    printf("✓ Stock data file 'stock_data.csv' created successfully!\n\n");

    // Load stock data from CSV
    load_stock_data(stocks, &stock_count);
    printf("✓ Loaded %d stocks with historical data\n\n", stock_count);

    // Main application loop
    int continue_running = 1;
    while (continue_running) {
        printf("\n╔═══════════════════════════════════════════════════════════════════════════╗\n");
        printf("║                            MAIN MENU                                      ║\n");
        printf("╚═══════════════════════════════════════════════════════════════════════════╝\n");
        printf("1. Strategy Management (Create/Edit/View/Delete)\n");
        printf("2. Run Backtest with Selected Strategy\n");
        printf("3. Compare All Strategies\n");
        printf("4. Logout\n");
        printf("\nEnter choice: ");
        
        int main_choice;
        scanf("%d", &main_choice);

        switch (main_choice) {
            case 1:
                strategy_management_menu(current_user, users, user_count);
                break;
                
            case 2: {
                Strategy strategy;
                Portfolio portfolio;
                double initial_cash = 100000.0;

                if (current_user->strategy_count > 0) {
                    printf("\n╔═══════════════════════════════════════════════════════════════════════════╗\n");
                    printf("║                        SELECT STRATEGY                                    ║\n");
                    printf("╚═══════════════════════════════════════════════════════════════════════════╝\n");
                    show_user_strategies(current_user);
                    strategy = select_user_strategy(current_user);
                    
                    // If user selected preset (flag = -1)
                    if (strategy.sma_short_period == -1) {
                        get_preset_strategy(&strategy);
                    }
                } else {
                    printf("\nNo custom strategies found. Please select a preset strategy.\n");
                    get_preset_strategy(&strategy);
                }

                // Initialize portfolio
                portfolio.cash = initial_cash;
                portfolio.trade_count = 0;
                for (int i = 0; i < MAX_STOCKS; i++) {
                    portfolio.positions[i] = 0;
                    portfolio.avg_buy_price[i] = 0.0;
                    portfolio.buy_day[i] = -1;
                }

                // Run backtest
                printf("\n╔═══════════════════════════════════════════════════════════════════════════╗\n");
                printf("║                         RUNNING BACKTEST                                  ║\n");
                printf("╚═══════════════════════════════════════════════════════════════════════════╝\n");
                printf("Strategy: %s\n", strategy.name);
                printf("Initial Capital: $%.2f\n", initial_cash);
                printf("Processing...\n\n");
                
                backtest(stocks, stock_count, strategy, &portfolio);

                // Print detailed results
                print_detailed_results(&portfolio, stocks, stock_count, initial_cash);
                
                printf("\nPress Enter to continue...");
                getchar();
                getchar();
                break;
            }
            
            case 3:
                run_comparison_backtest(stocks, stock_count, current_user, 100000.0);
                printf("\nPress Enter to continue...");
                getchar();
                getchar();
                break;
                
            case 4:
                printf("\n✓ Logging out...\n");
                printf("Thank you for using the Stock Backtesting System, %s!\n", logged_username);
                continue_running = 0;
                break;
                
            default:
                printf("\n❌ Invalid choice! Please try again.\n");
        }
    }

    return 0;
}