#ifndef LOAN_CALCULATOR_H
#define LOAN_CALCULATOR_H

namespace LoanCalculator {

// Calculates the monthly annuity payment
double calculateMonthlyPayment(
    double principal,
    double annualRate,
    int months
);

// Calculates the total repayment amount
double calculateTotalPayment(
    double principal,
    double annualRate,
    int months
);

// Calculates the total loan overpayment
double calculateOverpayment(
    double principal,
    double annualRate,
    int months
);

}

#endif
