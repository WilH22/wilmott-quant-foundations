#include <iostream>
#include <cmath> // std::exp
#include <iomanip> // std::setprecision
#include <string> // std::string
#include <utility> // std::pair
// Terminal workflow g++ xxxxxx.cpp -std=c++23 -O2 -o run && ./run

double forward_price(double S0, double r, double T) {
    // Theoritical forward price under continuous compounding:
    // F = S0 * exp(r*T)
    return S0 * std::exp(r*T);
}

std::string detect_arbitrage(double S0, double r, double T, double F_market) {
    double F_theoretical = forward_price (S0,r,T);

    if (F_market > F_theoretical){
        return "Cash-and-carry arbitrage (Forward overpriced)";}
    else if (F_market < F_theoretical){
        return "Reverse cash-and-carry arbitrage (Forward underpriced)";}
    else{
        return ("No arbitrage");}
}

std::pair<double, std::string> arbitrage_profit(double S0, double r, double T, double F_market, double tol= 1e-6) {
    double F_theoretical = forward_price(S0, r, T);
    double diff = F_market - F_theoretical;

    if (std::fabs(diff) <= tol) {
        return {0.0, "No arbitrage (within tolerance)"};
    } else if (diff > tol) {
        return {diff, "Cash-and-carry arbitrage (Forward overpriced)"};
    } else {
        return {-diff, "Reverse cash-and-carry arbitrage (Forward underpriced)"};
    }
}
int main () {
    double S0 = 100.0; // spot price today
    double r = 0.05; // 5% per year
    double T = 1.0; // 1 year
    double F_market = 120.0; // Market forward price

    double F = forward_price (S0, r, T);
    auto result = arbitrage_profit (S0, r, T, F_market);
    double profit = result.first;
    std::string label = result.second;

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Theoritical forward price: " << F << "\n";
    std::cout << detect_arbitrage(S0,r,T,F_market) << "\n";
    std::cout << "Arbitrage profit at maturity (per unit):" << profit << "\n";
    return 0 ;
}

