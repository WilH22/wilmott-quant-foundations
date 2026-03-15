#include <iostream>
#include <cmath> // std::exp
#include <iomanip> // std::setprecision
#include <string> // std::string
#include <utility> // std::pair
#include <fstream> // std::pl
#include <vector> // std::vector
#include <random> // std::normal_distribution
#include <algorithm> // std::max
// Terminal workflow g++ xxxxxx.cpp -std=c++23 -O2 -o run && ./run

// Theoritical forward price under continuous compounding and considering there is dividend:
// F = S0 * exp((r-q)*T)
double forward_price(double S0, double r, double y, double T) {
 
    return S0 * std::exp((r-y)*T);
}
// Detect Arbitrage Function
std::string detect_arbitrage(double S0, double r, double T, double y, double F_market) {
    double F_theoretical = forward_price (S0,r,y,T);

    if (F_market > F_theoretical){
        return "Cash-and-carry arbitrage (Forward overpriced)";}
    else if (F_market < F_theoretical){
        return "Reverse cash-and-carry arbitrage (Forward underpriced)";}
    else{
        return ("No arbitrage");}
}

// Grouping S0, r, y, T, F_Market 
struct Scenario {
    double S0, r, y, T, F_market;
};

// Arbitrage Profit Function
std::pair<double, std::string> arbitrage_profit(double S0, double r, double T, double y, double F_market, double tol= 1e-6) {
    double F_theoretical = forward_price(S0, r, y, T);
    double diff = F_market - F_theoretical;

    if (std::fabs(diff) <= tol) {
        return {0.0, "No arbitrage (within tolerance)"};
    } else if (diff > tol) {
        return {diff, "Cash-and-carry arbitrage (Forward overpriced)"};
    } else {
        return {-diff, "Reverse cash-and-carry arbitrage (Forward underpriced)"};
    }
}

// Forward Payoff Function
double forward_payoff (double S_T, double K) {
    return S_T - K;
}

// GBM Price Function
double gbm_price (double S0, double r, double y, double sigma, double T, double Z){
    return S0 * std::exp((r - y - 0.5 * sigma * sigma) * T + sigma * std::sqrt(T) * Z);
}


// GBM-to_Theoretical Price Comparison
struct GBM_Theoretical_Result{
    double simulated_mean;
    double theoretical_mean;
    double difference;
};

GBM_Theoretical_Result gbm_difference (double S0, double r, double y, double sigma, double T, int N) {
    // Random number generator
    std::mt19937 gen(std::random_device{}());
    std::normal_distribution<double> dist(0.0, 1.0);

    // Accumulate simulated terminal prices
    double sum_ST = 0.0;

    for (int i = 0; i < N; i++) {
        double Z = dist(gen);
        double ST = gbm_price (S0, r, y, sigma, T, Z);

        sum_ST += ST;
    }

    double simulated_mean = sum_ST / N;
    double theoretical_mean = S0 * std::exp((r-y) * T);
    double difference = simulated_mean - theoretical_mean;
    return {simulated_mean, theoretical_mean, difference};
}

// European Option Price Function
struct european_option_result{
    double average_payoff_call;
    double call_option_price;
    double average_payoff_put;
    double put_option_price;
};

european_option_result european_option_price (double S0, double r, double y, double T, double sigma, int N, double K) {
    // Random number generator
    std::mt19937 gen(std::random_device{}());
    std::normal_distribution<double> dist(0.0, 1.0);
    double total_payoff_call = 0.0;
    double total_payoff_put = 0.0;
    for (int i = 0; i < N; i++) {
        double Z = dist(gen);
        double ST = gbm_price (S0, r, y, sigma, T, Z);
        double payoff_call = std::max(ST-K, 0.0);
        total_payoff_call += payoff_call ;
        double payoff_put = std::max(K-ST, 0.0);
        total_payoff_put += payoff_put ;
    }
    double average_payoff_call = total_payoff_call / N;
    double call_option_price = average_payoff_call * std::exp(-r*T);
    double average_payoff_put = total_payoff_put / N;
    double put_option_price = average_payoff_put * std::exp(-r*T);
    return {average_payoff_call, call_option_price, average_payoff_put, put_option_price};
}

// Cummulative Distribution Function
double normal_cdf (double x){
    return 0.5 * (1.0 + erf(x / std::sqrt(2.0)));
}

struct black_scholes_result{
    double black_scholes_call;
    double black_scholes_put;
};

black_scholes_result black_scholes_price (double S0, double r, double y, double T, double sigma, double K) {
    
    double d1 = (log(S0/K) + (r -y + 0.5*sigma*sigma)*T) / (sigma*sqrt(T));
    double d2 = d1 - sigma*sqrt(T);

    double black_scholes_call = (S0*exp(-y*T) * normal_cdf(d1)) - (K * exp(-r*T) * normal_cdf(d2));
    double black_scholes_put =  (K * exp(-r*T) * normal_cdf(-d2)) - (S0*exp(-y*T) * normal_cdf(-d1));
    return {black_scholes_call, black_scholes_put};
}

// Asian Option Call Price Function
struct asian_option_result{
    double average_payoff_call;
    double call_option_price;
    double average_payoff_put;
    double put_option_price;
};

asian_option_result asian_option_price (double S0, double r, double y, double T, double sigma, int N, int M, double K) {
    double dt = T / M;
    // Random number generator
    std::mt19937 gen(std::random_device{}());
    std::normal_distribution<double> dist(0.0, 1.0);

    double total_payoff_call = 0.0;
    double total_payoff_put = 0.0;
    for (int i = 0; i < N; i++) {
        double ST = S0;
        double ST_sum = 0.0;
        for (int j = 0; j < M; j++) {
            double Z = dist(gen);
            ST = gbm_price(ST, r, y, sigma, dt, Z);
            ST_sum += ST;
        }
        double ST_mean = ST_sum / M;    
        double payoff_call = std::max(ST_mean - K, 0.0);
        total_payoff_call += payoff_call;
        double payoff_put = std::max(K - ST_mean, 0.0);
        total_payoff_put += payoff_put; 
    }
    double average_payoff_call = total_payoff_call / N;
    double call_option_price = average_payoff_call * std::exp(-r*T);
    double average_payoff_put = total_payoff_put / N;
    double put_option_price = average_payoff_put * std::exp(-r*T);
    return {average_payoff_call, call_option_price, average_payoff_put, put_option_price};
}

// MAIN
int main () {
    double S0 = 100.0; // spot price today
    double r = 0.05; // 5% per year
    double T = 1.0; // 1 year
    int M = 12; // Number of time steps in each simulated price path
    double F_market = 120.0; // Market forward price
    double y = 0.02; // 0.3% dividend per year
    double F = forward_price (S0, r, y, T);
    double sigma = 0.30; // Volatility
    int N = 100000; // Number of trial
    double K = 100.0; // Strike price
    auto result = arbitrage_profit (S0, r, T, y, F_market);
    double profit = result.first;
    std::string label = result.second;

    // Detection for forward price
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Theoretical forward price: " << F << "\n";
    std::cout << detect_arbitrage(S0,r,T,y,F_market) << "\n";
    std::cout << "Arbitrage profit at maturity (per unit):" << profit << "\n";
    
    double S_theoretical = forward_price (S0, r, y, T);
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
    std::cout << "Fair forward price (Theoretical Price) = " << S_theoretical << "\n";

    // Mulitple Cases
    std::vector<Scenario> cases = {
        {100.0, 0.05, 0.00, 1.0, 120.0},
        {100.0, 0.05, 0.02, 1.0, 105.0},
        {100.0, 0.03, 0.04, 0.5, 98.0}
    };
    std::cout <<std::fixed << std::setprecision(4);
    std::cout << std::setw(12) << "S0"
          << std::setw(10) << "r"
          << std::setw(10) << "y"
          << std::setw(10) << "T"
          << std::setw(12) << "F_mkt"
          << std::setw(12) << "F_theory"
          << std::setw(12) << "mispricing"
          << "\n";

    for (const auto& sc : cases){
        double F_theory = forward_price (sc.S0, sc.r, sc.y, sc.T);
        double mispricing = sc.F_market - F_theory;

        std::cout << std::setw(12) << sc.S0
              << std::setw(10) << sc.r
              << std::setw(10) << sc.y
              << std::setw(10) << sc.T
              << std::setw(12) << sc.F_market
              << std::setw(12) << F_theory
              << std::setw(12) << mispricing
              << "\n";
        }
    
    // GBM-to-theoretical Comparison 
    GBM_Theoretical_Result gbm = gbm_difference (S0, r, y, sigma, T, N);
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "\n" << "Simulated E[ST]   :" << gbm.simulated_mean << "\n";
    std::cout << "Theoretical E[ST] :" << gbm.theoretical_mean << "\n";
    std::cout << "Difference        :" << gbm.difference << "\n";

    // European option call price
    european_option_result euro = european_option_price (S0, r, y, T, sigma, N, K);
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "\n" << "European Average option payoff (Call) :" << euro.average_payoff_call << "\n";
    std::cout << "European Call option price            :" << euro.call_option_price << "\n";
    std::cout << "European Average option payoff (Put)  :" << euro.average_payoff_put << "\n";
    std::cout << "European Put option price             :" << euro.put_option_price << "\n";
    
    // Black Scholes Price
    black_scholes_result bs = black_scholes_price (S0, r, y, T, sigma, K);
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "\n";
    std::cout << "Black-Scholes Option Call price       :" << bs.black_scholes_call << "\n";
    std::cout << "Black-Scholes Option Put price        :" << bs.black_scholes_put  << "\n";
    
    // Asian option call price
    asian_option_result asia = asian_option_price (S0, r, y, T, sigma, N, M, K);
    std::cout << "\n" << "Asian Average option payoff (Call)    :" << asia.average_payoff_call << "\n";
    std::cout << "Asian Call option price               :" << asia.call_option_price << "\n";
    std::cout << "Asian Average option payoff (Put)     :" << asia.average_payoff_put << "\n";
    std::cout << "Asian Put option price                :" << asia.put_option_price << "\n";
    return 0 ;
}

