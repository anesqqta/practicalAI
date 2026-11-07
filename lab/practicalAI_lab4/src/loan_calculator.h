#ifndef LOAN_CALCULATOR_H
#define LOAN_CALCULATOR_H

namespace LoanCalculator 
{
    /**
     * @brief Contains functions for calculating loan payments
     */

    /**
     * @brief Calculates the monthly annuity loan payment
     * @param principal The initial loan amount
     * @param annualRate The annual interest rate in percent
     * @param months The loan repayment period in months
     * @return The monthly payment rounded to two decimal places
     * @throws std::invalid_argument If the input parameters are invalid
     */
    double calculateMonthlyPayment(double principal, double annualRate, int months);

    /**
     * @brief Calculates the total loan repayment amount
     * @param principal The initial loan amount
     * @param annualRate The annual interest rate in percent
     * @param months The loan repayment period in months
     * @return The total repayment amount rounded to two decimal places
     * @throws std::invalid_argument If the input parameters are invalid
     */
    double calculateTotalPayment(double principal, double annualRate, int months);

    /**
     * @brief Calculates the total loan overpayment
     * @param principal The initial loan amount
     * @param annualRate The annual interest rate in percent
     * @param months The loan repayment period in months
     * @return The overpayment amount rounded to two decimal places
     * @throws std::invalid_argument If the input parameters are invalid
     */
    double calculateOverpayment(double principal, double annualRate, int months);
}

#endif // LOAN_CALCULATOR_H
