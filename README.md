# Wilmott Quant Foundations

This repository documents my structured study of:

> **Paul Wilmott – Introduces Quantitative Finance**

I implement concepts step-by-step in both **Python** and **C++**, focusing on building intuition, pricing consistency, and arbitrage logic.

---

## 📌 Current Progress

### Chapter 1 – Products and Markets

#### Implemented:
- Forward pricing under continuous compounding (cost-of-carry framework)
- Dividend-adjusted forward valuation: ST = S0 * exp((r - y) * T)
- Arbitrage detection:
  - Cash-and-carry
  - Reverse cash-and-carry
- Arbitrage profit computation at maturity (per unit)
- Multi-scenario forward mispricing analysis (C++ & Python)
- Floating-point tolerance handling for arbitrage detection
- Payoff profile visualization and CSV export pipeline
- Stochastic processes implementation (Monte Carlo European and Asian Option Price Call-Put)
- Black-Scholes option price Call-Put 
  
#### Languages Used:
- Python (Notebook implementation)
- C++ (Console application)

---

## 🧠 Concepts Covered

- Spot vs Forward pricing
- No-arbitrage principle
- Cost-of-carry model
- Risk-free discounting
- Arbitrage mechanics
- Stochastic processes
- Geometric Brownian Motion (GBM)
- Black-Scholes Pricing
- Monte Carlo Simulation
- Call-Put Option Pricing
---

## 🚀 Next Steps


### Medium-Term
- Portfolio risk metrics (VaR)
- Volatility modeling

### Long-Term
- Greeks and sensitivity analysis
- Structured risk modelling engine

---

---

## 🎯 Goal

Build a structured, research-oriented quantitative finance foundation
with production-style implementations in both Python and C++.
