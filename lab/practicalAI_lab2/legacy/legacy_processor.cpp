#include <iostream>
#include <string>
#include <iomanip>
#include <clocale>
#include <windows.h>

using namespace std;

// Глобальні змінні
double insuranceRate = 0.025;
double processingFee = 0.015;
double totalProcessed = 0.0;
int processedLoans = 0;
bool useInsurance = true;

// Параметри окремого типу кредиту
struct LoanRules {
    double commissionRate;
    double amountThreshold;
    double increasedCommissionRate;

    double baseAdditionalFee;
    double rateThreshold;
    double rateAdditionalFee;

    double insuranceMultiplier;

    int serviceMonthsThreshold;
    double serviceAdditionalFee;

    double earlyRepaymentFactor;
    double earlyRepaymentFee;
};

// Отримання параметрів відповідно до типу кредиту
bool getLoanRules(const string& loanType, LoanRules& rules) {
    if (loanType == "CONSUMER") {
        rules = {
            0.02, 100000, 0.025,
            500,  20,     300,
            1.0,  60,     250,
            0.9,  200
        };
    }
    else if (loanType == "MORTGAGE") {
        rules = {
            0.01, 1000000, 0.015,
            1500, 15,      1000,
            0.8,  120,     750,
            0.95, 500
        };
    }
    else if (loanType == "AUTO") {
        rules = {
            0.03, 500000, 0.035,
            800,  18,     500,
            1.2,  84,     400,
            0.85, 300
        };
    }
    else {
        return false;
    }

    return true;
}

// Перевірка коректності вхідних даних
bool validateLoanInput(double amount, double annualRate, int months) {
    if (amount <= 0) {
        cout << "Помилка: сума кредиту повинна бути додатною." << endl;
        return false;
    }

    if (annualRate < 0 || annualRate > 100) {
        cout << "Помилка: некоректна відсоткова ставка." << endl;
        return false;
    }

    if (months <= 0 || months > 360) {
        cout << "Помилка: некоректний термін кредитування." << endl;
        return false;
    }

    return true;
}

// Розрахунок початкової комісії та додаткових зборів
void calculateLoanTypeFees(
    const LoanRules& rules,
    double amount,
    double annualRate,
    double& commission,
    double& additionalFee
) {
    commission = amount * rules.commissionRate;
    additionalFee = rules.baseAdditionalFee;

    if (annualRate > rules.rateThreshold) {
        additionalFee += rules.rateAdditionalFee;
    }

    if (amount > rules.amountThreshold) {
        commission = amount * rules.increasedCommissionRate;
    }
}

// Розрахунок щомісячного платежу
double calculateMonthlyPayment(double amount, double monthlyRate, int months) {
    if (monthlyRate == 0) {
        return amount / months;
    }

    double denominator = 1;

    for (int i = 0; i < months - 1; i++) {
        denominator *= (1 + monthlyRate);
    }

    double power = denominator * (1 + monthlyRate);

    return amount * monthlyRate * power / (power - 1);
}

// Розрахунок вартості страхування
double calculateInsurance(const LoanRules& rules, double amount, bool insurance) {
    if (insurance && useInsurance) {
        return amount * insuranceRate * rules.insuranceMultiplier;
    }

    return 0.0;
}

// Комісія за обслуговування терміну кредиту
void calculateServiceCommission(
    const LoanRules& rules,
    double amount,
    int months,
    double& commission
) {
    commission += amount * processingFee;

    if (months > rules.serviceMonthsThreshold) {
        commission += rules.serviceAdditionalFee;
    }
}

// Обробка дострокового погашення
void applyEarlyRepayment(
    const LoanRules& rules,
    bool earlyRepayment,
    double& totalInterest,
    double& additionalFee
) {
    if (earlyRepayment) {
        totalInterest *= rules.earlyRepaymentFactor;
        additionalFee += rules.earlyRepaymentFee;
    }
}

// Виведення результатів розрахунку
void printLoanResult(
    const string& loanType,
    double amount,
    double monthlyPayment,
    double totalInterest,
    double commission,
    double insuranceCost,
    double additionalFee,
    double totalAmount,
    bool earlyRepayment
) {
    if (totalAmount > amount * 2) {
        cout << "Увага: загальна вартість кредиту перевищує тіло кредиту вдвічі!" << endl;
    }

    if (insuranceCost > 0) {
        cout << "Страхування включено." << endl;
    }
    else {
        cout << "Страхування не включено." << endl;
    }

    if (earlyRepayment) {
        cout << "Застосовано опцію дострокового погашення." << endl;
    }
    else {
        cout << "Стандартні умови погашення." << endl;
    }

    cout << fixed << setprecision(2);
    cout << "\n----- РОЗРАХУНОК КРЕДИТУ -----" << endl;
    cout << "Тип кредиту: " << loanType << endl;
    cout << "Тіло кредиту (сума): " << amount << endl;
    cout << "Щомісячний платіж: " << monthlyPayment << endl;
    cout << "Загальні відсотки: " << totalInterest << endl;
    cout << "Комісія: " << commission << endl;
    cout << "Страховка: " << insuranceCost << endl;
    cout << "Додаткові збори: " << additionalFee << endl;
    cout << "Загальна сума до сплати: " << totalAmount << endl;
}

// Основна функція обробки кредиту
double processLoan(
    string loanType,
    double amount,
    double annualRate,
    int months,
    bool earlyRepayment,
    bool insurance
) {
    if (!validateLoanInput(amount, annualRate, months)) {
        return -1;
    }

    LoanRules rules;

    if (!getLoanRules(loanType, rules)) {
        cout << "Помилка: невідомий тип кредиту." << endl;
        return -1;
    }

    double commission = 0.0;
    double insuranceCost = 0.0;
    double additionalFee = 0.0;
    double monthlyRate = annualRate / 12.0 / 100.0;

    calculateLoanTypeFees(rules, amount, annualRate, commission, additionalFee);
    
    double monthlyPayment = calculateMonthlyPayment(amount, monthlyRate, months);
    double totalInterest = monthlyPayment * months - amount;

    insuranceCost = calculateInsurance(rules, amount, insurance);
    calculateServiceCommission(rules, amount, months, commission);
    applyEarlyRepayment(rules, earlyRepayment, totalInterest, additionalFee);

    double totalAmount = amount + totalInterest + commission + insuranceCost + additionalFee;

    totalProcessed += totalAmount;
    processedLoans++;

    printLoanResult(
        loanType, amount, monthlyPayment, totalInterest,
        commission, insuranceCost, additionalFee, totalAmount, earlyRepayment
    );

    return totalAmount;
}

int main() {
    // Встановлення української локалі для коректного виведення
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    setlocale(LC_ALL, "Ukrainian_Ukraine.65001");

    cout << "=== СИСТЕМА РОЗРАХУНКУ КРЕДИТІВ ===" << endl;

    double amount;
    double rate;
    int months;
    string type;
    char earlyChoice;
    char insuranceChoice;

    cout << "Введіть тип кредиту (CONSUMER, MORTGAGE, AUTO): ";
    cin >> type;

    cout << "Введіть суму кредиту: ";
    cin >> amount;

    cout << "Введіть річну відсоткову ставку (%): ";
    cin >> rate;

    cout << "Введіть термін кредиту (в місяцях): ";
    cin >> months;

    cout << "Чи планується дострокове погашення? (y/n): ";
    cin >> earlyChoice;

    cout << "Включити страхування? (y/n): ";
    cin >> insuranceChoice;

    bool earlyRepayment = (earlyChoice == 'y' || earlyChoice == 'Y');
    bool insurance = (insuranceChoice == 'y' || insuranceChoice == 'Y');

    double result = processLoan(type, amount, rate, months, earlyRepayment, insurance);

    if (result > 0) {
        cout << "\nРозрахунок завершено успішно." << endl;
    }
    else {
        cout << "\nПомилка розрахунку." << endl;
    }

    cout << "\nОброблено кредитів: " << processedLoans << endl;
    cout << "Загальна сума всіх оброблених кредитів: " << totalProcessed << endl;

    return 0;
}
