#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <iomanip>
#include <fstream>
#include <sstream>

struct Stock {
    std::string name;
    double purchasePrice;
    double currentPrice;
    std::vector<double> historicalPrices;
    double averagePriceChange;
    double volume; // Daily trading volume
};

bool isStrongPassword(const std::string& password) {
    std::regex uppercaseRegex("[A-Z]");
    std::regex lowercaseRegex("[a-z]");
    std::regex digitRegex("[0-9]");
    std::regex specialCharRegex("[!@#\\$%^&*()_+\\-=\\[\\]{};':\",.<>?]");

    return (std::regex_search(password, uppercaseRegex) &&
            std::regex_search(password, lowercaseRegex) &&
            std::regex_search(password, digitRegex) &&
            std::regex_search(password, specialCharRegex) &&
            password.length() >= 8);
}

std::string recommendAction(const Stock& stock) {
    double profitMargin = (stock.currentPrice - stock.purchasePrice) / stock.purchasePrice * 100;

    // Consider average historical price change
    if (stock.averagePriceChange > 0.5) {
        return "Buy (Positive Average Price Change)";
    }

    // Consider trading volume
    if (stock.volume > 1000000) {
        return "Buy (High Trading Volume)";
    }

    // Default recommendation based on profit margin
    if (profitMargin > 5.0) {
        return "Buy";
    } else if (profitMargin < -5.0) {
        return "Sell";
    } else {
        return "Hold";
    }
}

void calculateAdditionalFactors(Stock& stock) {
    // Calculate average historical price change
    if (stock.historicalPrices.size() > 1) {
        double totalChange = 0.0;
        for (size_t i = 1; i < stock.historicalPrices.size(); ++i) {
            totalChange += std::abs(stock.historicalPrices[i] - stock.historicalPrices[i - 1]);
        }
        stock.averagePriceChange = totalChange / (stock.historicalPrices.size() - 1);
    }

    // Simulate daily trading volume (example data)
    stock.volume = rand() % 2000000 + 500000;  // Random value between 500,000 and 2,000,000
}

void displayStockDetails(const Stock& stock) {
    std::cout << "Details for " << stock.name << ":" << std::endl;
    std::cout << "Purchase Price: $" << stock.purchasePrice << std::endl;
    std::cout << "Current Price: $" << stock.currentPrice << std::endl;
    std::cout << "Historical Stock Prices for the Last 10 Days:" << std::endl;
    for (double price : stock.historicalPrices) {
        std::cout << std::fixed << std::setprecision(2) << price << " ";
    }
    std::cout << std::endl;
    std::cout << "Average Price Change: $" << std::fixed << std::setprecision(2) << stock.averagePriceChange << std::endl;
    std::cout << "Daily Trading Volume: " << stock.volume << std::endl;
}

// Function to read stock data from a CSV file
std::vector<Stock> readStockData(const std::string& filename) {
    std::vector<Stock> stocks;

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening the file." << std::endl;
        return stocks;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Stock stock;

        // Assuming CSV format: Name,PurchasePrice,CurrentPrice,HistoricalPrices (comma-separated)
        std::getline(iss, stock.name, ',');
        iss >> stock.purchasePrice;
        iss.ignore();  // Ignore the comma
        iss >> stock.currentPrice;
        iss.ignore();  // Ignore the comma

        double price;
        while (iss >> price) {
            stock.historicalPrices.push_back(price);
            iss.ignore();  // Ignore the comma
        }

        calculateAdditionalFactors(stock);
        stocks.push_back(stock);
    }

    file.close();
    return stocks;
}

double calculateProfitLoss(const Stock& stock) {
    return (stock.currentPrice - stock.purchasePrice);
}

double calculateAverageDailyTradingVolume(const std::vector<Stock>& stocks) {
    double totalVolume = 0.0;
    size_t stockCount = stocks.size();

    for (const auto& stock : stocks) {
        totalVolume += stock.volume;
    }

    return (stockCount > 0) ? (totalVolume / stockCount) : 0.0;
}

double calculateNetTradingVolume(const std::vector<Stock>& stocks) {
    double netVolume = 0.0;

    for (const auto& stock : stocks) {
        netVolume += (stock.currentPrice - stock.purchasePrice) * stock.volume;
    }

    return netVolume;
}

#include <cstdio> // Add this include for popen

void plotProfitMargins(const std::vector<Stock>& stocks) {
    // Prepare data file for Gnuplot
    std::ofstream dataFile("profit_margins.csv");
    for (size_t i = 0; i < stocks.size(); ++i) {
        double profitMargin = (stocks[i].currentPrice - stocks[i].purchasePrice) / stocks[i].purchasePrice * 100;
        dataFile << i + 1 << " " << profitMargin << std::endl;
    }
    dataFile.close();

    // Gnuplot command
    std::string gnuplotCommand = "gnuplot";
    std::string script = "plot 'profit_margins.dat' with linespoints title 'Profit Margin'";
    std::string command = gnuplotCommand + " -e '" + script + "' -p"; // -p flag opens a new window

    // Open a pipe to the gnuplot process
    FILE *gnuplotPipe = popen(command.c_str(), "w");

    if (!gnuplotPipe) {
        std::cerr << "Error opening pipe to Gnuplot." << std::endl;
        return;
    }

    // Close the pipe
    pclose(gnuplotPipe);
}
int main() {
    std::string username, password;

    // Registration
    std::cout << "Registration Page" << std::endl;
    std::cout << "Enter your username: ";
    std::cin >> username;

    // Password complexity check
    do {
        std::cout << "Enter a strong password (at least 8 characters with uppercase, lowercase, digit, special character): ";
        std::cin >> password;
    } while (!isStrongPassword(password));

    std::cout << "Registration Successful" << std::endl;

    // Login
    std::string enteredUsername, enteredPassword;

    std::cout << "Login Page" << std::endl;
    std::cout << "Enter your username: ";
    std::cin >> enteredUsername;
    std::cout << "Enter your password: ";
    std::cin >> enteredPassword;

    if (username == enteredUsername && password == enteredPassword) {
        std::cout << "Login Successful" << std::endl;

        // Read stock information from a CSV file
        std::vector<Stock> stocks = readStockData("data.csv");

        int choice;
        do {
            // Display menu
            std::cout << "\nMenu:\n";
            std::cout << "1. View Stock Details\n";
            std::cout << "2. Calculate Profit/Loss\n";
            std::cout << "3. Calculate Average Daily Trading Volume\n";
            std::cout << "4. Calculate Net Trading Volume\n";
            std::cout << "5. Visualize Profit Margins\n";
            std::cout << "0. Exit\n";
            std::cout << "Enter your choice: ";
            std::cin >> choice;

            switch (choice) {
                case 1: {
                    // View stock details
                    int stockChoice;
                    std::cout << "Enter the number of the stock to view details: ";
                    std::cin >> stockChoice;

                    // Check if the choice is valid
                    if (stockChoice >= 1 && static_cast<size_t>(stockChoice) <= stocks.size()) {
                        const Stock& selectedStock = stocks[stockChoice - 1];
                        displayStockDetails(selectedStock);
                        std::string action = recommendAction(selectedStock);
                        std::cout << "Recommendation: " << action << " (Profit Margin: " << std::fixed << std::setprecision(2)
                                << ((selectedStock.currentPrice - selectedStock.purchasePrice) / selectedStock.purchasePrice) * 100
                                << "%)" << std::endl;
                    } else {
                        std::cout << "Invalid choice. Please enter a valid stock number." << std::endl;
                    }
                    break;
                }
                case 2: {
                    // Calculate profit/loss
                    int stockChoice;
                    std::cout << "Enter the number of the stock for profit/loss calculation: ";
                    std::cin >> stockChoice;

                    // Check if the choice is valid
                    if (stockChoice >= 1 && static_cast<size_t>(stockChoice) <= stocks.size()) {
                        const Stock& selectedStock = stocks[stockChoice - 1];
                        double profitLoss = calculateProfitLoss(selectedStock);
                        std::cout << "Profit/Loss: $" << std::fixed << std::setprecision(2) << profitLoss << std::endl;
                    } else {
                        std::cout << "Invalid choice. Please enter a valid stock number." << std::endl;
                    }
                    break;
                }
                case 3: {
                    // Calculate average daily trading volume
                    double avgDailyVolume = calculateAverageDailyTradingVolume(stocks);
                    std::cout << "Average Daily Trading Volume: $" << std::fixed << std::setprecision(0) << avgDailyVolume << std::endl;
                    break;
                }
                case 4: {
                    // Calculate net trading volume
                    double netTradingVolume = calculateNetTradingVolume(stocks);
                    std::cout << "Net Trading Volume: $" << std::fixed << std::setprecision(2) << netTradingVolume << std::endl;
                    break;
                }
                case 5: {
                    // Visualize profit margins
                    plotProfitMargins(stocks);
                    std::cout << "Profit margins visualization created (profit_margins_plot.png).\n";
                    break;
                }
                case 0:
                    std::cout << "Exiting the program. Goodbye!\n";
                    break;
                default:
                    std::cout << "Invalid choice. Please enter a valid option.\n";
            }
        } while (choice != 0);
    } else {
        std::cout << "Login Failed. Please try again." << std::endl;
    }

    return 0;
}