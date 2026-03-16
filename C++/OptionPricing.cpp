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
// Probability Function
double normal_cdf (double x){
    return 0.5 * (1.0 + erf(x / std::sqrt(2.0)));
}
double normal_pdf(double x) {
    const double inv_sqrt_2pi = 0.3989422804014327; //0.3989422804014327 = 1 / sqrt(2π) 
    return inv_sqrt_2pi * std::exp(-0.5 * x * x);
}

//Parity RHS Function
double parity_lhs_function(double call_price, double put_price) {
    return call_price - put_price;
}
struct MCSummary {
    double price;
    double std_error;
    double ci_low;
    double ci_high;
};


MCSummary mc_summary(const std::vector<double>& payoffs, double r, double T) {
    const int n = static_cast<int>(payoffs.size());

    std::vector<double> discounted_payoffs;
    discounted_payoffs.reserve(n);

    const double discount = std::exp(-r * T);

    for (double payoff : payoffs) {
        discounted_payoffs.push_back(discount * payoff);
    }

    double sum = std::accumulate(discounted_payoffs.begin(), discounted_payoffs.end(), 0.0);
    double price = sum / n;

    double sq_sum = 0.0;
    for (double x : discounted_payoffs) {
        sq_sum += x * x;
    }

    double variance = 0.0;
    if (n > 1) {
        variance = (sq_sum - (sum * sum) / n) / (n - 1);
    }

    double std_error = std::sqrt(variance / n);
    double ci_low = price - 1.96 * std_error;
    double ci_high = price + 1.96 * std_error;

    return {price, std_error, ci_low, ci_high};
}
// European Option Pricing
struct european_option_result {
    double call_option_price;
    double call_std_error;
    double call_ci_high;
    double call_ci_low;

    double put_option_price;
    double put_std_error;
    double put_ci_high;
    double put_ci_low;

    double parity_lhs;
};

european_option_result european_option_price (double S0, double r, double y, double T, double sigma, int N, double K) {
    // Random number generator
    std::mt19937 gen(std::random_device{}());
    std::normal_distribution<double> dist(0.0, 1.0);
    double total_payoff_call = 0.0;
    double total_payoff_put = 0.0;
    std::vector<double> payoffs_call;
    std::vector<double> payoffs_put;
    for (int i = 0; i < N; i++) {
        double Z = dist(gen);
        double ST = gbm_price (S0, r, y, sigma, T, Z);
        double payoff_call = std::max(ST-K, 0.0);
        double payoff_put = std::max(K-ST, 0.0);
        payoffs_call.push_back(payoff_call);
        payoffs_put.push_back(payoff_put);
    }
    MCSummary call_summary =  mc_summary(payoffs_call, r, T);
    MCSummary put_summary =  mc_summary(payoffs_put, r, T);
    double parity_lhs = parity_lhs_function(call_summary.price, put_summary.price);
    return {
        call_summary.price, call_summary.std_error, call_summary.ci_high, call_summary.ci_low, 
        put_summary.price, put_summary.std_error, put_summary.ci_high, put_summary.ci_low, parity_lhs};
}


// Black-Scholes Option Pricing
struct black_scholes_result{
    double black_scholes_call;
    double black_scholes_put;
    double parity_lhs;
};

black_scholes_result black_scholes_price (double S0, double r, double y, double T, double sigma, double K) {
    
    double d1 = (log(S0/K) + (r -y + 0.5*sigma*sigma)*T) / (sigma*sqrt(T));
    double d2 = d1 - sigma*sqrt(T);

    double black_scholes_call = (S0*exp(-y*T) * normal_cdf(d1)) - (K * exp(-r*T) * normal_cdf(d2));
    double black_scholes_put =  (K * exp(-r*T) * normal_cdf(-d2)) - (S0*exp(-y*T) * normal_cdf(-d1));
    double parity_lhs = parity_lhs_function(black_scholes_call, black_scholes_put);
    return {black_scholes_call, black_scholes_put, parity_lhs};
}
struct bs_greeks {
    double delta_call;
    double delta_put;
    double gamma;
    double vega;
    double theta_call;
    double theta_put;
};

bs_greeks black_scholes_greeks(double S0, double K, double r, double y, double sigma, double T){
    double d1 = (log(S0 / K) + (r - y + 0.5 * sigma*sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);

    double pdf_d1 = normal_pdf(d1);
    
    double delta_call = exp(-y*T) * normal_cdf(d1);
    double delta_put = -exp(-y*T) * normal_cdf(-d1);

    double gamma = exp(-y*T) * normal_pdf(d1) / (S0 * sigma * std::sqrt(T));

    double vega = S0 * exp(-y*T) * normal_pdf(d1) * std::sqrt(T);

    double theta_call = (
        -S0 * normal_pdf(d1) * sigma * exp (-y*T) / (2*std::sqrt(T))
        - r * K * exp(-r*T) * normal_cdf(d2)
        + y * S0 * exp(-y*T) * normal_cdf(d1)
    );
    double theta_put = (
        -S0 * normal_pdf(d1) * sigma * exp (-y*T) / (2*std::sqrt(T))
        + r * K * exp(-r*T) * normal_cdf(-d2)
        - y * S0 * exp(-y*T) * normal_cdf(-d1)
    );
    return {delta_call, delta_put, gamma, vega, theta_call, theta_put};
}


// Asian Option Price Function
struct asian_option_result {
    double call_option_price;
    double call_std_error;
    double call_ci_high;
    double call_ci_low;

    double put_option_price;
    double put_std_error;
    double put_ci_high;
    double put_ci_low;

    double parity_lhs;
};

asian_option_result asian_option_price (double S0, double r, double y, double T, double sigma, int N, int M, double K) {
    double dt = T / M;
    // Random number generator
    std::mt19937 gen(std::random_device{}());
    std::normal_distribution<double> dist(0.0, 1.0);

    double total_payoff_call = 0.0;
    double total_payoff_put = 0.0;
    std::vector<double> payoffs_call;
    std::vector<double> payoffs_put;

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
        double payoff_put = std::max(K - ST_mean, 0.0);
        payoffs_call.push_back(payoff_call);
        payoffs_put.push_back(payoff_put);}
    MCSummary call_summary =  mc_summary(payoffs_call, r, T);
    MCSummary put_summary =  mc_summary(payoffs_put, r, T);
    double parity_lhs = parity_lhs_function(call_summary.price, put_summary.price);
    return {
        call_summary.price, call_summary.std_error, call_summary.ci_high, call_summary.ci_low, 
        put_summary.price, put_summary.std_error, put_summary.ci_high, put_summary.ci_low, parity_lhs};
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
    std::vector<double> payoffs_call_std;
    std::vector<double> payoffs_put_std;
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
    
    // Option Pricing
    // GBM-to-theoretical Comparison 
    GBM_Theoretical_Result gbm = gbm_difference (S0, r, y, sigma, T, N);
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "\n" << "Simulated E[ST]   :" << gbm.simulated_mean << "\n";
    std::cout << "Theoretical E[ST] :" << gbm.theoretical_mean << "\n";
    std::cout << "Difference        :" << gbm.difference << "\n";


    // Black-Scholes Pricing + Greeks
    black_scholes_result bs = black_scholes_price(S0, r, y, T, sigma, K);
    bs_greeks greeks = black_scholes_greeks(S0, K, r, y, sigma, T);

    // Monte Carlo Pricing
    european_option_result euro = european_option_price(S0, r, y, T, sigma, N, K);
    asian_option_result asia = asian_option_price(S0, r, y, T, sigma, N, M, K);

    // Put-Call Parity RHS
    double parity_rhs = S0 * std::exp(-y * T) - K * std::exp(-r * T);

    std::cout << std::fixed << std::setprecision(4);

    // ==============================
    // Black-Scholes
    // ==============================
    std::cout << "\n========================================";
    std::cout << "\nBlack-Scholes Option Pricing";
    std::cout << "\n========================================\n";

    std::cout << "Call Price                  : " << bs.black_scholes_call << "\n";
    std::cout << "Put Price                   : " << bs.black_scholes_put  << "\n";

    std::cout << "\nGreeks\n";
    std::cout << "Delta Call                  : " << greeks.delta_call << "\n";
    std::cout << "Delta Put                   : " << greeks.delta_put << "\n";
    std::cout << "Gamma                       : " << greeks.gamma << "\n";
    std::cout << "Theta Call                  : " << greeks.theta_call << "\n";
    std::cout << "Theta Put                   : " << greeks.theta_put << "\n";

    // ==============================
    // Monte Carlo European
    // ==============================
    std::cout << "\n========================================";
    std::cout << "\nMonte Carlo (European) Option Pricing";
    std::cout << "\n========================================\n";

    std::cout << "Simulations                 : " << N << "\n";
    std::cout << "Call Price                  : " << euro.call_option_price << "\n";
    std::cout << "Std Error (Call)            : " << euro.call_std_error << "\n";
    std::cout << "95% CI (Call)               : [" << euro.call_ci_low << ", " << euro.call_ci_high << "]\n";

    std::cout << "\nPut Price                   : " << euro.put_option_price << "\n";
    std::cout << "Std Error (Put)             : " << euro.put_std_error << "\n";
    std::cout << "95% CI (Put)                : [" << euro.put_ci_low << ", " << euro.put_ci_high << "]\n";

    // ==============================
    // Monte Carlo Asian
    // ==============================
    std::cout << "\n========================================";
    std::cout << "\nMonte Carlo (Asian) Option Pricing";
    std::cout << "\n========================================\n";

    std::cout << "Simulations                 : " << N << "\n";
    std::cout << "Time Steps                  : " << M << "\n";
    std::cout << "Call Price                  : " << asia.call_option_price << "\n";
    std::cout << "Std Error (Call)            : " << asia.call_std_error << "\n";
    std::cout << "95% CI (Call)               : [" << asia.call_ci_low << ", " << asia.call_ci_high << "]\n";

    std::cout << "\nPut Price                   : " << asia.put_option_price << "\n";
    std::cout << "Std Error (Put)             : " << asia.put_std_error << "\n";
    std::cout << "95% CI (Put)                : [" << asia.put_ci_low << ", " << asia.put_ci_high << "]\n";

    // ==============================
    // Put-Call Parity Check
    // ==============================
    std::cout << "\n========================================";
    std::cout << "\nPut-Call Parity Check";
    std::cout << "\n========================================\n";

    std::cout << "RHS = S0*exp(-yT) - K*exp(-rT) : " << parity_rhs << "\n";

    std::cout << "\nBlack-Scholes\n";
    std::cout << "LHS = C - P                    : " << bs.parity_lhs << "\n";
    std::cout << "Difference (LHS - RHS)         : " << bs.parity_lhs - parity_rhs << "\n";

    std::cout << "\nMonte Carlo (European)\n";
    std::cout << "LHS = C - P                    : " << euro.parity_lhs << "\n";
    std::cout << "Difference (LHS - RHS)         : " << euro.parity_lhs - parity_rhs << "\n";

    std::cout << "\nMonte Carlo (Asian)\n";
    std::cout << "LHS = C - P                    : " << asia.parity_lhs << "\n";
    std::cout << "Difference (LHS - RHS)         : " << asia.parity_lhs - parity_rhs << "\n";
    std::cout << "Note                           : Standard put-call parity does not apply to Asian options.\n";

    return 0;
    }

