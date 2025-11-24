#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user_management.h"
#include "structures.h"

void load_users(User users[], int *user_count) {
    FILE *fp = fopen("users.csv", "r");
    if (fp == NULL) {
        *user_count = 0;
        return;
    }

    char line[2048];
    fgets(line, sizeof(line), fp); // Skip header
    *user_count = 0;

    while (fgets(line, sizeof(line), fp) && *user_count < MAX_USERS) {
        User *user = &users[*user_count];
        char *token;
        
        // Parse username
        token = strtok(line, ",");
        if (token) strcpy(user->username, token);
        
        // Parse password
        token = strtok(NULL, ",");
        if (token) strcpy(user->password, token);
        
        // Parse strategy count
        token = strtok(NULL, ",");
        if (token) user->strategy_count = atoi(token);
        
        // Parse each strategy
        for (int i = 0; i < user->strategy_count && i < MAX_STRATEGIES_PER_USER; i++) {
            Strategy *s = &user->custom_strategies[i];
            
            token = strtok(NULL, "|");
            if (token) strcpy(s->name, token);
            
            token = strtok(NULL, "|");
            if (token) s->rsi_oversold = atof(token);
            
            token = strtok(NULL, "|");
            if (token) s->rsi_overbought = atof(token);
            
            token = strtok(NULL, "|");
            if (token) s->sma_short_period = atoi(token);
            
            token = strtok(NULL, "|");
            if (token) s->sma_long_period = atoi(token);
            
            token = strtok(NULL, "|");
            if (token) s->stop_loss_pct = atof(token);
            
            token = strtok(NULL, "|");
            if (token) s->take_profit_pct = atof(token);
            
            token = strtok(NULL, ",");
            if (token) s->max_holding_days = atoi(token);
        }
        
        (*user_count)++;
    }

    fclose(fp);
}

void save_users(User users[], int user_count) {
    FILE *fp = fopen("users.csv", "w");
    if (fp == NULL) {
        printf("Error saving users!\n");
        return;
    }

    // Write header
    fprintf(fp, "Username,Password,StrategyCount,Strategies\n");

    // Write each user
    for (int i = 0; i < user_count; i++) {
        User *user = &users[i];
        fprintf(fp, "%s,%s,%d", user->username, user->password, user->strategy_count);
        
        // Write each strategy separated by commas, fields within strategy separated by pipes
        for (int j = 0; j < user->strategy_count; j++) {
            Strategy *s = &user->custom_strategies[j];
            fprintf(fp, ",%s|%.2f|%.2f|%d|%d|%.2f|%.2f|%d",
                    s->name, s->rsi_oversold, s->rsi_overbought,
                    s->sma_short_period, s->sma_long_period,
                    s->stop_loss_pct, s->take_profit_pct, s->max_holding_days);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

void register_user(User users[], int *user_count) {
    User new_user;
    printf("\n=== USER REGISTRATION ===\n");
    printf("Enter username: ");
    scanf("%s", new_user.username);
    
    // Check if username already exists
    for (int i = 0; i < *user_count; i++) {
        if (strcmp(users[i].username, new_user.username) == 0) {
            printf("Username already exists! Please try a different username.\n");
            return;
        }
    }
    
    printf("Enter password: ");
    scanf("%s", new_user.password);
    
    new_user.strategy_count = 0;
    users[*user_count] = new_user;
    (*user_count)++;
    
    printf("User registered successfully!\n");
}

int login(User users[], int user_count, char *logged_username) {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    
    printf("\n=== USER LOGIN ===\n");
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0 && 
            strcmp(users[i].password, password) == 0) {
            strcpy(logged_username, username);
            return i;
        }
    }
    
    return -1;
}

void create_new_strategy(User *user) {
    if (user->strategy_count >= MAX_STRATEGIES_PER_USER) {
        printf("Maximum strategies reached! Please delete a strategy first.\n");
        return;
    }
    
    Strategy *strategy = &user->custom_strategies[user->strategy_count];
    
    printf("\n=== CREATE NEW STRATEGY ===\n");
    printf("Enter strategy name: ");
    scanf(" %[^\n]", strategy->name);
    printf("Enter RSI oversold level (0-100, e.g., 30): ");
    scanf("%lf", &strategy->rsi_oversold);
    printf("Enter RSI overbought level (0-100, e.g., 70): ");
    scanf("%lf", &strategy->rsi_overbought);
    printf("Enter short SMA period in days (e.g., 5): ");
    scanf("%d", &strategy->sma_short_period);
    printf("Enter long SMA period in days (e.g., 20): ");
    scanf("%d", &strategy->sma_long_period);
    printf("Enter stop loss %% (e.g., 5.0): ");
    scanf("%lf", &strategy->stop_loss_pct);
    printf("Enter take profit %% (e.g., 10.0): ");
    scanf("%lf", &strategy->take_profit_pct);
    printf("Enter max holding days (e.g., 15): ");
    scanf("%d", &strategy->max_holding_days);
    
    user->strategy_count++;
    printf("\n✓ Strategy '%s' created successfully!\n", strategy->name);
}

void edit_strategy(User *user) {
    if (user->strategy_count == 0) {
        printf("\nNo strategies to edit!\n");
        return;
    }
    
    show_user_strategies(user);
    
    int choice;
    printf("\nSelect strategy to edit (1-%d): ", user->strategy_count);
    scanf("%d", &choice);
    
    if (choice < 1 || choice > user->strategy_count) {
        printf("Invalid choice!\n");
        return;
    }
    
    Strategy *strategy = &user->custom_strategies[choice - 1];
    
    printf("\n=== EDITING STRATEGY: %s ===\n", strategy->name);
    printf("Current values are shown in [brackets]\n\n");
    
    char temp[50];
    printf("Enter new strategy name [%s] (or press Enter to keep): ", strategy->name);
    scanf(" %[^\n]", temp);
    if (strlen(temp) > 0) strcpy(strategy->name, temp);
    
    printf("Enter RSI oversold level [%.2f]: ", strategy->rsi_oversold);
    if (scanf("%lf", &strategy->rsi_oversold) != 1) {
        while(getchar() != '\n');
    }
    
    printf("Enter RSI overbought level [%.2f]: ", strategy->rsi_overbought);
    if (scanf("%lf", &strategy->rsi_overbought) != 1) {
        while(getchar() != '\n');
    }
    
    printf("Enter short SMA period [%d]: ", strategy->sma_short_period);
    if (scanf("%d", &strategy->sma_short_period) != 1) {
        while(getchar() != '\n');
    }
    
    printf("Enter long SMA period [%d]: ", strategy->sma_long_period);
    if (scanf("%d", &strategy->sma_long_period) != 1) {
        while(getchar() != '\n');
    }
    
    printf("Enter stop loss %% [%.2f]: ", strategy->stop_loss_pct);
    if (scanf("%lf", &strategy->stop_loss_pct) != 1) {
        while(getchar() != '\n');
    }
    
    printf("Enter take profit %% [%.2f]: ", strategy->take_profit_pct);
    if (scanf("%lf", &strategy->take_profit_pct) != 1) {
        while(getchar() != '\n');
    }
    
    printf("Enter max holding days [%d]: ", strategy->max_holding_days);
    if (scanf("%d", &strategy->max_holding_days) != 1) {
        while(getchar() != '\n');
    }
    
    printf("\n✓ Strategy '%s' updated successfully!\n", strategy->name);
}

void show_user_strategies(User *user) {
    printf("\n=== YOUR SAVED STRATEGIES ===\n");
    if (user->strategy_count == 0) {
        printf("No strategies saved yet.\n");
        return;
    }
    
    for (int i = 0; i < user->strategy_count; i++) {
        Strategy *s = &user->custom_strategies[i];
        printf("\n%d. %s\n", i + 1, s->name);
        printf("   RSI: %.0f-%.0f | SMA: %d/%d | Stop Loss: %.1f%% | Take Profit: %.1f%% | Max Days: %d\n",
               s->rsi_oversold, s->rsi_overbought, s->sma_short_period, s->sma_long_period,
               s->stop_loss_pct, s->take_profit_pct, s->max_holding_days);
    }
}

void delete_strategy(User *user) {
    if (user->strategy_count == 0) {
        printf("\nNo strategies to delete!\n");
        return;
    }
    
    show_user_strategies(user);
    
    int choice;
    printf("\nSelect strategy to delete (1-%d): ", user->strategy_count);
    scanf("%d", &choice);
    
    if (choice < 1 || choice > user->strategy_count) {
        printf("Invalid choice!\n");
        return;
    }
    
    char confirm;
    printf("Are you sure you want to delete '%s'? (y/n): ", 
           user->custom_strategies[choice - 1].name);
    scanf(" %c", &confirm);
    
    if (confirm == 'y' || confirm == 'Y') {
        // Shift strategies down
        for (int i = choice - 1; i < user->strategy_count - 1; i++) {
            user->custom_strategies[i] = user->custom_strategies[i + 1];
        }
        user->strategy_count--;
        printf("\n✓ Strategy deleted successfully!\n");
    } else {
        printf("\nDeletion cancelled.\n");
    }
}

Strategy select_user_strategy(User *user) {
    int choice;
    printf("\nSelect strategy (1-%d) or 0 for preset strategies: ", user->strategy_count);
    scanf("%d", &choice);
    
    if (choice > 0 && choice <= user->strategy_count) {
        return user->custom_strategies[choice - 1];
    } else {
        Strategy strategy;
        // Return empty strategy to indicate preset selection needed
        strategy.sma_short_period = -1; // Flag for main to handle
        return strategy;
    }
}

void strategy_management_menu(User *user, User users[], int user_count) {
    int choice;
    
    do {
        printf("\n=== STRATEGY MANAGEMENT ===\n");
        printf("1. Create New Strategy\n");
        printf("2. Edit Existing Strategy\n");
        printf("3. View All Strategies\n");
        printf("4. Delete Strategy\n");
        printf("5. Back to Main Menu\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                create_new_strategy(user);
                save_users(users, user_count);
                break;
            case 2:
                edit_strategy(user);
                save_users(users, user_count);
                break;
            case 3:
                show_user_strategies(user);
                break;
            case 4:
                delete_strategy(user);
                save_users(users, user_count);
                break;
            case 5:
                printf("Returning to main menu...\n");
                break;
            default:
                printf("Invalid choice!\n");
        }
    } while (choice != 5);
}