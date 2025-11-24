#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "stock_data.h"
#include "structures.h"

void create_sample_csv() {
    FILE *fp = fopen("stock_data.csv", "w");
    if (fp == NULL) {
        printf("Error creating CSV file!\n");
        return;
    }

    fprintf(fp, "Symbol,Date,Open,High,Low,Close,Volume\n");

    srand(time(NULL));

    // Stock 1: TECH_A
    double tech_a_base = 100.0;
    for (int i = 0; i < 50; i++) {
        double change = (rand() % 600 - 300) / 100.0;
        tech_a_base += change;
        double open = tech_a_base;
        double close = tech_a_base + (rand() % 400 - 200) / 100.0;
        double high = (open > close ? open : close) + (rand() % 200) / 100.0;
        double low = (open < close ? open : close) - (rand() % 200) / 100.0;
        int volume = 100000 + (rand() % 50000);
        fprintf(fp, "TECH_A,2024-01-%02d,%.2f,%.2f,%.2f,%.2f,%d\n", 
                i + 1, open, high, low, close, volume);
    }

    // Stock 2: FINANCE_B
    double fin_b_base = 150.0;
    for (int i = 0; i < 50; i++) {
        double change = (rand() % 800 - 400) / 100.0;
        fin_b_base += change;
        double open = fin_b_base;
        double close = fin_b_base + (rand() % 500 - 250) / 100.0;
        double high = (open > close ? open : close) + (rand() % 250) / 100.0;
        double low = (open < close ? open : close) - (rand() % 250) / 100.0;
        int volume = 80000 + (rand() % 40000);
        fprintf(fp, "FINANCE_B,2024-01-%02d,%.2f,%.2f,%.2f,%.2f,%d\n", 
                i + 1, open, high, low, close, volume);
    }

    // Stock 3: ENERGY_C
    double energy_c_base = 75.0;
    for (int i = 0; i < 50; i++) {
        double change = (rand() % 500 - 250) / 100.0;
        energy_c_base += change;
        double open = energy_c_base;
        double close = energy_c_base + (rand() % 300 - 150) / 100.0;
        double high = (open > close ? open : close) + (rand() % 150) / 100.0;
        double low = (open < close ? open : close) - (rand() % 150) / 100.0;
        int volume = 120000 + (rand() % 60000);
        fprintf(fp, "ENERGY_C,2024-01-%02d,%.2f,%.2f,%.2f,%.2f,%d\n", 
                i + 1, open, high, low, close, volume);
    }

    fclose(fp);
}

void load_stock_data(Stock stocks[], int *stock_count) {
    FILE *fp = fopen("stock_data.csv", "r");
    if (fp == NULL) {
        printf("Error opening CSV file!\n");
        return;
    }

    char line[256];
    fgets(line, sizeof(line), fp);

    char current_symbol[MAX_STOCK_NAME] = "";
    int stock_idx = -1;

    while (fgets(line, sizeof(line), fp)) {
        char symbol[MAX_STOCK_NAME];
        PriceData data;

        sscanf(line, "%[^,],%[^,],%lf,%lf,%lf,%lf,%d",
               symbol, data.date, &data.open, &data.high, &data.low, &data.close, &data.volume);

        if (strcmp(current_symbol, symbol) != 0) {
            stock_idx++;
            strcpy(stocks[stock_idx].symbol, symbol);
            strcpy(current_symbol, symbol);
            stocks[stock_idx].day_count = 0;
        }

        strcpy(stocks[stock_idx].prices[stocks[stock_idx].day_count].date, data.date);
        stocks[stock_idx].prices[stocks[stock_idx].day_count].open = data.open;
        stocks[stock_idx].prices[stocks[stock_idx].day_count].high = data.high;
        stocks[stock_idx].prices[stocks[stock_idx].day_count].low = data.low;
        stocks[stock_idx].prices[stocks[stock_idx].day_count].close = data.close;
        stocks[stock_idx].prices[stocks[stock_idx].day_count].volume = data.volume;
        stocks[stock_idx].day_count++;
    }

    *stock_count = stock_idx + 1;
    fclose(fp);
}