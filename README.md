# Wilmott Quant Foundations

This repository documents my structured study of:

**Paul Wilmott — Introduces Quantitative Finance**

The goal of this project is to build a **strong conceptual foundation in quantitative finance by implementing models directly from theory into code.**

The focus is on **learning and intuition**, rather than production-ready code.

Each concept from the book is translated into **working implementations in both Python and C++**, allowing exploration of numerical behavior, arbitrage mechanics, and stochastic modeling.

This repository serves as a **learning laboratory** for core quantitative finance concepts.

A separate repository will later be developed for a **clean, production-style option pricing engine.**

---

# 🚀 Current Progress

## Chapter 1 – Products and Markets

### Forward Pricing & Arbitrage

Implemented the **cost-of-carry model** for forward valuation and tested arbitrage opportunities under different market conditions.

Forward pricing with dividend yield:

\[
F_0 = S_0 e^{(r-y)T}
\]

Features implemented:

- Forward pricing under continuous compounding
- Dividend-adjusted forward valuation
- Cash-and-carry arbitrage detection
- Reverse cash-and-carry arbitrage detection
- Arbitrage profit calculation at maturity
- Multi-scenario forward mispricing analysis
- Floating point tolerance handling
- CSV export for payoff analysis

---

# 📈 Stochastic Price Modeling

### Geometric Brownian Motion (GBM)

Implemented stochastic simulation of asset prices using the GBM model:

\[S_T = S_0 * exp(r - y - (sigma^2)/2) * T + sigma * sqrt(T) * Z)\]

Implemented features:

- GBM simulation
- Monte Carlo estimation of terminal prices
- Empirical validation of the theoretical expectation

\[
E[S_T] = S_0 e^{(r-y)T}
\]

---

# 💰 Option Pricing

## Monte Carlo Methods

Monte Carlo simulation is used to estimate option prices under GBM dynamics.

Implemented:

- European Call pricing
- European Put pricing
- Asian Call pricing
- Asian Put pricing

Features included:

- Path simulation
- Standard error estimation
- Confidence intervals
- Antithetic variates for variance reduction

---

## Analytical Pricing

### Black–Scholes Model

Closed-form pricing for European options using the Black–Scholes model:

\[
C = S_0 e^{-yT} N(d_1) - K e^{-rT} N(d_2)
\]

Implemented:

- Call pricing
- Put pricing
- Greeks calculation

Greeks currently implemented:

- Delta
- Gamma
- Theta

---

# 🔍 Model Validation

### Put–Call Parity

European options must satisfy the no-arbitrage condition:

\[
C - P = S_0 e^{-yT} - K e^{-rT}
\]

Observed results:

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
- Variance reduction (Antithetic Variates)
- Black-Scholes option pricing
- Greeks
- Path-dependent derivatives (Asian options)
- Put–Call Parity validation

---

# 🛠 Languages Used

**Python**

- Jupyter notebooks for experimentation
- Visualization and statistical validation

**C++**

- Console implementation
- Focus on numerical understanding and performance

---

# 🔜 Next Steps

## Short Term
- Monte Carlo convergence analysis
- Volatility sensitivity studies
- Strike sensitivity analysis
- Option price visualization

## Medium Term
- Control variates for variance reduction
- Monte Carlo Greeks estimation
- Variance comparison across estimators

## Long Term
- Stochastic calculus implementations
- PDE-based option pricing
- Finite difference methods
- Multi-asset derivatives pricing

---

# Future Project

This repository focuses on **learning and experimentation**.

A separate repository will be developed for a **clean, modular option pricing engine**, including:

- reusable pricing interfaces
- multiple stochastic models
- advanced variance reduction
- calibration tools
- performance optimization

---
