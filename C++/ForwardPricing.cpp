#include <iostream>
#include <cmath> // std::exp
#include <iomanip> // std::setprecision
#include <string> // std::string
#include <utility> // std::pair
#include <fstream> // std::pl
// Terminal workflow g++ xxxxxx.cpp -std=c++23 -O2 -o run && ./run

double forward_price(double S0, double r, double q, double T) {
    // Theoritical forward price under continuous compounding and considering there is dividend:
    // F = S0 * exp((r-q)*T)
    return S0 * std::exp((r-q)*T);
}

std::string detect_arbitrage(double S0, double r, double T, double q, double F_market) {
    double F_theoretical = forward_price (S0,r,q,T);

    if (F_market > F_theoretical){
        return "Cash-and-carry arbitrage (Forward overpriced)";}
    else if (F_market < F_theoretical){
        return "Reverse cash-and-carry arbitrage (Forward underpriced)";}
    else{
        return ("No arbitrage");}
}

std::pair<double, std::string> arbitrage_profit(double S0, double r, double T, double q, double F_market, double tol= 1e-6) {
    double F_theoretical = forward_price(S0, r, q, T);
    double diff = F_market - F_theoretical;

    if (std::fabs(diff) <= tol) {
        return {0.0, "No arbitrage (within tolerance)"};
    } else if (diff > tol) {
        return {diff, "Cash-and-carry arbitrage (Forward overpriced)"};
    } else {
        return {-diff, "Reverse cash-and-carry arbitrage (Forward underpriced)"};
    }
}

double forward_payoff (double S_T, double K) {
    return S_T - K;
}

int main () {
    double S0 = 100.0; // spot price today
    double r = 0.05; // 5% per year
    double T = 1.0; // 1 year
    double F_market = 120.0; // Market forward price
    double q = 0.3; // 0.3% dividend per year
    double F = forward_price (S0, r, q, T);
    auto result = arbitrage_profit (S0, r, T, q, F_market);
    double profit = result.first;
    std::string label = result.second;

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Theoritical forward price: " << F << "\n";
    std::cout << detect_arbitrage(S0,r,T,q,F_market) << "\n";
    std::cout << "Arbitrage profit at maturity (per unit):" << profit << "\n";
    
    double K = forward_price (S0, r, q, T);
    std::ofstream file("output/Forward_payoff.csv");
    file << "S_T,payoff\n";
    
    for (int i = 50; i<=150; i++) {
        double S_T = static_cast<double>(i);
        double payoff = forward_payoff (S_T, K);
        file << S_T << "," << payoff << "\n";
    }

    file.close();
    std::cout << "CSV write done.\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Generated forward_payoff.csv\n";
    std::cout << "Fair forward K = " << K << "\n";
    return 0 ;
}

