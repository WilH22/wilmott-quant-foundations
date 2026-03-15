# Wilmott Quant Foundations

This repository documents my structured study of:

**Paul Wilmott — Introduces Quantitative Finance**

The goal of this project is to implement key quantitative finance concepts step-by-step in **Python and C++**, focusing on building intuition through code.

The project evolves progressively from **forward pricing and arbitrage detection** to **stochastic modeling and option pricing**.

---

# 🚀 Current Progress

## Chapter 1 – Products and Markets

### Implemented

#### Forward Pricing & Arbitrage
- Forward pricing under continuous compounding (cost-of-carry model)
- Dividend-adjusted forward valuation:

\[
F_0 = S_0 e^{(r-y)T}
\]

- Arbitrage detection:
  - Cash-and-carry
  - Reverse cash-and-carry
- Arbitrage profit calculation at maturity
- Multi-scenario forward mispricing analysis
- Floating-point tolerance handling
- CSV export for payoff analysis

---

### Stochastic Price Modeling

- Geometric Brownian Motion (GBM) simulation
- Monte Carlo estimation of terminal stock price
- Validation of simulated expectation:

\[
E[S_T] = S_0 e^{(r-y)T}
\]

---

### Option Pricing

#### Monte Carlo Methods
- European Call pricing
- European Put pricing
- Asian Call pricing
- Asian Put pricing

#### Analytical Pricing
- Black-Scholes formula for European options

\[
C = S_0 e^{-yT} N(d_1) - K e^{-rT} N(d_2)
\]

---

### Model Validation

#### Put-Call Parity Verification

European options satisfy the no-arbitrage condition:

\[
C - P = S_0 e^{-yT} - K e^{-rT}
\]

Results from the project show:

- **Black-Scholes pricing satisfies parity exactly**
- **Monte Carlo European pricing approximates parity (simulation noise)**
- **Asian options do not satisfy standard parity due to path dependency**

---

# 🧠 Concepts Covered

- Spot vs Forward pricing
- No-arbitrage principle
- Cost-of-carry model
- Risk-free discounting
- Arbitrage mechanics
- Geometric Brownian Motion (GBM)
- Monte Carlo simulation
- Black-Scholes option pricing
- Path-dependent derivatives (Asian options)
- Put-Call Parity

---

# 🛠 Languages Used

- **Python** – notebook implementation and visualization  
- **C++** – console implementation focused on performance and numerical understanding

---

# 📊 Example Output
Python:

---

# 🔜 Next Steps

### Short Term
- Greeks implementation (Delta, Gamma, Vega, Theta)

### Medium Term
- Monte Carlo convergence analysis
- Volatility sensitivity studies
- Option price vs strike / volatility visualization

### Long Term
- Stochastic calculus implementation
- PDE-based option pricing
- Multi-asset derivatives pricing
