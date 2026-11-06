#include <gtest/gtest.h>

#include "loan_calculator.h"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <tuple>

// Parameterized test data:
// principal, annual rate, months, expected monthly payment
using PaymentCase = std::tuple<double, double, int, double>;

class MonthlyPaymentTest : public ::testing::TestWithParam<PaymentCase> {
};

TEST_P(MonthlyPaymentTest, CalculatesMonthlyPaymentCorrectly) {
    const auto [principal, annualRate, months, expected] = GetParam();

    const double result = LoanCalculator::calculateMonthlyPayment(
        principal, annualRate, months
    );

    EXPECT_NEAR(result, expected, 0.01);
}

INSTANTIATE_TEST_SUITE_P(
    ValidLoanCases,
    MonthlyPaymentTest,
    ::testing::Values(
        PaymentCase{12000.0, 0.0, 12, 1000.00},
        PaymentCase{120000.0, 12.0, 12, 10661.85},
        PaymentCase{1000.0, 100.0, 1, 1083.33},
        PaymentCase{50000.0, 0.0, 50, 1000.00}
    )
);

// Total repayment tests

TEST(LoanCalculatorTest, CalculatesTotalPaymentCorrectly) {
    const double result = LoanCalculator::calculateTotalPayment(
        120000.0, 12.0, 12
    );

    EXPECT_NEAR(result, 127942.20, 0.01);
}

TEST(LoanCalculatorTest, CalculatesTotalPaymentForZeroInterest) {
    const double result = LoanCalculator::calculateTotalPayment(
        12000.0, 0.0, 12
    );

    EXPECT_NEAR(result, 12000.00, 0.01);
}

// Overpayment tests

TEST(LoanCalculatorTest, CalculatesOverpaymentCorrectly) {
    const double result = LoanCalculator::calculateOverpayment(
        120000.0, 12.0, 12
    );

    EXPECT_NEAR(result, 7942.20, 0.01);
}

TEST(LoanCalculatorTest, CalculatesZeroOverpaymentForZeroInterest) {
    const double result = LoanCalculator::calculateOverpayment(
        12000.0, 0.0, 12
    );

    EXPECT_NEAR(result, 0.00, 0.01);
}

// Invalid principal tests

TEST(LoanCalculatorTest, RejectsInvalidPrincipal) {
    EXPECT_THROW(
        LoanCalculator::calculateMonthlyPayment(0.0, 12.0, 12),
        std::invalid_argument
    );

    EXPECT_THROW(
        LoanCalculator::calculateMonthlyPayment(-1000.0, 12.0, 12),
        std::invalid_argument
    );

    EXPECT_THROW(
        LoanCalculator::calculateMonthlyPayment(10000001.0, 12.0, 12),
        std::invalid_argument
    );
}

// Invalid interest rate tests

TEST(LoanCalculatorTest, RejectsInvalidInterestRate) {
    EXPECT_THROW(
        LoanCalculator::calculateMonthlyPayment(10000.0, -1.0, 12),
        std::invalid_argument
    );

    EXPECT_THROW(
        LoanCalculator::calculateMonthlyPayment(10000.0, 100.1, 12),
        std::invalid_argument
    );
}

// Invalid loan term tests

TEST(LoanCalculatorTest, RejectsInvalidLoanTerm) {
    EXPECT_THROW(
        LoanCalculator::calculateMonthlyPayment(10000.0, 12.0, 0),
        std::invalid_argument
    );

    EXPECT_THROW(
        LoanCalculator::calculateMonthlyPayment(10000.0, 12.0, -1),
        std::invalid_argument
    );

    EXPECT_THROW(
        LoanCalculator::calculateMonthlyPayment(10000.0, 12.0, 361),
        std::invalid_argument
    );
}

// Non-finite input tests

TEST(LoanCalculatorTest, RejectsNonFinitePrincipal) {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double infinity = std::numeric_limits<double>::infinity();

    EXPECT_THROW(
        LoanCalculator::calculateMonthlyPayment(nan, 12.0, 12),
        std::invalid_argument
    );

    EXPECT_THROW(
        LoanCalculator::calculateMonthlyPayment(infinity, 12.0, 12),
        std::invalid_argument
    );
}

TEST(LoanCalculatorTest, RejectsNonFiniteInterestRate) {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double infinity = std::numeric_limits<double>::infinity();

    EXPECT_THROW(
        LoanCalculator::calculateMonthlyPayment(10000.0, nan, 12),
        std::invalid_argument
    );

    EXPECT_THROW(
        LoanCalculator::calculateMonthlyPayment(10000.0, infinity, 12),
        std::invalid_argument
    );
}

// Validation in other public functions

TEST(LoanCalculatorTest, TotalPaymentRejectsInvalidInput) {
    EXPECT_THROW(
        LoanCalculator::calculateTotalPayment(0.0, 12.0, 12),
        std::invalid_argument
    );
}

TEST(LoanCalculatorTest, OverpaymentRejectsInvalidInput) {
    EXPECT_THROW(
        LoanCalculator::calculateOverpayment(10000.0, -1.0, 12),
        std::invalid_argument
    );
}
