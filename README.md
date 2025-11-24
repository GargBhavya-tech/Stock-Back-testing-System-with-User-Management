# Stock-Back-testing-System-with-User-Management
A comprehensive C-based stock backtesting system with user authentication, 
custom strategy management, and detailed performance analytics.

## Features

### 🔐 User Authentication
- User registration and login system
- Secure credential storage in CSV format
- Multi-user support with individual strategy profiles

### 📊 Strategy Management
- **Create** custom trading strategies with configurable parameters
- **Edit** existing strategies with current value display
- **View** all saved strategies with detailed parameters
- **Delete** unwanted strategies
- Support for up to 10 strategies per user

### 💹 Trading Strategies
**Preset Strategies:**
1. **SMA Crossover** - Simple Moving Average crossover signals
2. **RSI Strategy** - Relative Strength Index based trading
3. **Combined Strategy** - Hybrid approach using both SMA and RSI

**Custom Strategy Parameters:**
- RSI oversold/overbought levels
- Short and long-term SMA periods
- Stop loss percentage
- Take profit percentage
- Maximum holding period

### 🎯 Backtesting Engine
- Historical stock data simulation
- Multiple stock support (TECH_A, FINANCE_B, ENERGY_C)
- Real-time trade execution simulation
- Portfolio management with position tracking
- Detailed trade history with reasoning

### 📈 Performance Analytics
- Complete trade-by-trade breakdown
- Profit/loss calculations
- Win rate statistics
- Portfolio value tracking
- Open position monitoring
- Strategy comparison reports

## Project Structure

```
project/
├── structures.h          - Data structure definitions
├── user_management.h     - User management declarations
├── user_management.c     - User authentication & strategy CRUD
├── stock_data.h          - Stock data declarations
├── stock_data.c          - CSV generation and loading
├── backtest.h            - Backtesting declarations
├── backtest.c            - Core backtesting engine
├── main.c                - Main program entry point
├── Makefile              - Build configuration
└── README.md             - This file
```

## File Descriptions

### Header Files
- **structures.h**: Defines all data structures (User, Strategy, Portfolio, etc.)
- **user_management.h**: User authentication and strategy management functions
- **stock_data.h**: Stock data handling and CSV operations
- **backtest.h**: Backtesting algorithms and analysis functions

### Implementation Files
- **user_management.c**: 
  - CSV-based user storage
  - Login/registration
  - Strategy CRUD operations (Create, Read, Update, Delete)
  
- **stock_data.c**:
  - Random stock data generation
  - CSV parsing and loading
  - Historical price data management

- **backtest.c**:
  - SMA and RSI calculation
  - Trade signal generation
  - Portfolio simulation
  - Performance metrics calculation
  - Strategy comparison

- **main.c**:
  - Program flow control
  - Menu system
  - User interaction

## Compilation

### Using Make (Recommended)
```bash
# Build the program
make

# Build and run
make run

# Clean object files
make clean

# Clean all generated files
make cleanall

# Show help
make help
```

### Manual Compilation
```bash
gcc -Wall -Wextra -std=c99 -g -c main.c
gcc -Wall -Wextra -std=c99 -g -c user_management.c
gcc -Wall -Wextra -std=c99 -g -c stock_data.c
gcc -Wall -Wextra -std=c99 -g -c backtest.c
gcc -Wall -Wextra -std=c99 -g -o backtest_system main.o user_management.o stock_data.o backtest.o -lm
```

## Usage

### First Time Setup
1. Compile the program: `make`
2. Run the program: `./backtest_system`
3. Register a new account
4. Login with your credentials

### Creating a Custom Strategy
1. Login to your account
2. Select "Strategy Management"
3. Choose "Create New Strategy"
4. Enter strategy parameters:
   - Strategy name
   - RSI levels (0-100)
   - SMA periods (days)
   - Stop loss % (recommended: 3-10%)
   - Take profit % (recommended: 5-20%)
   - Max holding days (recommended: 5-30)

### Editing a Strategy
1. Go to "Strategy Management"
2. Select "Edit Existing Strategy"
3. Choose the strategy to edit
4. Modify parameters (press Enter to keep current value)

### Running a Backtest
1. Select "Run Backtest with Selected Strategy"
2. Choose from your custom strategies or preset strategies
3. Review detailed results including:
   - Trade-by-trade history
   - Portfolio summary
   - Trading statistics
   - Open positions

### Comparing Strategies
1. Select "Compare All Strategies"
2. System will backtest:
   - All 3 preset strategies
   - All your custom strategies
3. View side-by-side comparison with rankings

## Data Files

### users.csv
Stores user credentials and strategies in CSV format:
Username,Password,StrategyCount,Strategies
john,pass123,2,MyStrategy1|30|70|5|20|5.0|10.0|15,Aggressive|25|75|3|10|7.0|15.0|10

### stock_data.csv
Generated stock price data:
Symbol,Date,Open,High,Low,Close,Volume
TECH_A,2024-01-01,100.00,102.50,99.50,101.00,125000

## Strategy Parameters Explained

### RSI (Relative Strength Index)
- **Oversold Level** (20-40): Buy signal when RSI drops below this
- **Overbought Level** (60-80): Sell signal when RSI rises above this
- Common: 30 (oversold) and 70 (overbought)

### SMA (Simple Moving Average)
- **Short Period** (3-10 days): Fast-responding average
- **Long Period** (15-50 days): Slow-responding average
- Buy when short crosses above long; sell when opposite

### Risk Management
- **Stop Loss**: Exit position when loss reaches this % (prevents large losses)
- **Take Profit**: Exit position when profit reaches this % (locks in gains)
- **Max Holding**: Maximum days to hold position (prevents dead money)

## Example Strategies

### Conservative Strategy
- RSI: 25/75
- SMA: 10/30
- Stop Loss: 3%
- Take Profit: 7%
- Max Days: 20

### Aggressive Strategy
- RSI: 35/65
- SMA: 5/15
- Stop Loss: 7%
- Take Profit: 15%
- Max Days: 10

### Balanced Strategy
- RSI: 30/70
- SMA: 5/20
- Stop Loss: 5%
- Take Profit: 12%
- Max Days: 15

## Troubleshooting

### Compilation Errors
- Ensure all .h and .c files are in the same directory
- Check that gcc is installed: `gcc --version`
- Use `-lm` flag for math library

### Runtime Issues
- **"Error opening CSV file"**: Run from correct directory
- **"Login failed"**: Check username/password or register new account
- **"Maximum strategies reached"**: Delete unused strategies

### Data Issues
- Delete `users.csv` to reset user database
- Delete `stock_data.csv` to regenerate stock data
- Use `make cleanall` to remove all data files

## Performance Tips

1. **Backtest Period**: Current system uses 50 days of data
2. **Initial Capital**: Default $100,000 (modify in main.c)
3. **Position Sizing**: Each trade uses 20% of available cash
4. **Multiple Stocks**: System tests 3 stocks simultaneously
