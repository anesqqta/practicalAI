#include "loan_calculator.h"

#include <cmath>
#include <stdexcept>

namespace LoanCalculator {

namespace {

// Input limits.
constexpr double kMinPrincipal = 0.0;
constexpr double kMaxPrincipal = 10000000.0;
constexpr double kMinAnnualRate = 0.0;
constexpr double kMaxAnnualRate = 100.0;
constexpr int kMinMonths = 1;
constexpr int kMaxMonths = 360;

// Validates all input parameters.
void validateInputs(double principal, double annualRate, int months) {
    if (!std::isfinite(principal)) {
        throw std::invalid_argument("Principal must be a finite number.");
    }

    if (principal <= kMinPrincipal) {
        throw std::invalid_argument("Principal must be greater than zero.");
    }

    if (principal > kMaxPrincipal) {
        throw std::invalid_argument("Principal exceeds the maximum limit.");
    }

    if (!std::isfinite(annualRate)) {
        throw std::invalid_argument("Interest rate must be a finite number.");
    }

    if (annualRate < kMinAnnualRate) {
        throw std::invalid_argument("Interest rate cannot be negative.");
    }

    if (annualRate > kMaxAnnualRate) {
        throw std::invalid_argument("Interest rate exceeds 100 percent.");
    }

    if (months < kMinMonths) {
        throw std::invalid_argument("Loan term must be at least one month.");
    }

    if (months > kMaxMonths) {
        throw std::invalid_argument("Loan term cannot exceed 360 months.");
    }
}

// Rounds a monetary value to two decimal places.
double roundToCents(double value) {
    return std::round(value * 100.0) / 100.0;
}

} // namespace

double calculateMonthlyPayment(
    double principal,
    double annualRate,
    int months
) {
    validateInputs(principal, annualRate, months);

    const double monthlyRate = annualRate / 12.0 / 100.0;

    double payment;

    if (monthlyRate == 0.0) {
        payment = principal / months;
    } else {
        payment = principal * monthlyRate /
                  (1.0 - std::pow(1.0 + monthlyRate, -months));
    }

    return roundToCents(payment);
}

double calculateTotalPayment(
    double principal,
    double annualRate,
    int months
) {
    const double monthlyPayment = calculateMonthlyPayment(
        principal, annualRate, months
    );

    return roundToCents(monthlyPayment * months);
}

double calculateOverpayment(
    double principal,
    double annualRate,
    int months
) {
    const double totalPayment = calculateTotalPayment(
        principal, annualRate, months
    );

    return roundToCents(totalPayment - principal);
}

} // namespace LoanCalculator
