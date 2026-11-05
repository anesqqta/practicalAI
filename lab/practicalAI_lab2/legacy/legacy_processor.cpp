#include <iostream>
#include <string>
#include <iomanip>
#include <clocale>
#include <windows.h>

using namespace std;

// Фіксовані категорії кредитів
enum class LoanType {
 Consumer,
 Mortgage,
 Auto
};

// Налаштування системи кредитування
struct LoanSettings {
 double insuranceRate = 0.025;
 double processingFee = 0.015;
 bool useInsurance = true;
};

// Статистика обробки кредитів
struct LoanStatistics {
 double totalProcessed = 0.0;
 int processedLoans = 0;
};

// Вхідні параметри кредиту
struct LoanInput {
 LoanType type;
 double amount;
 double annualRate;
 int months;
 bool earlyRepayment;
 bool insurance;
};

// Правила розрахунку для окремого типу кредиту
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

// Результати розрахунку кредиту
struct LoanResult {
 double monthlyPayment = 0.0;
 double totalInterest = 0.0;
 double commission = 0.0;
 double insuranceCost = 0.0;
 double additionalFee = 0.0;
 double totalAmount = 0.0;
 bool success = false;
};

// Об'єкти налаштувань і статистики
LoanSettings settings;
LoanStatistics loanStatistics;

// Посилання для сумісності з попередніми регресійними тестами
double& insuranceRate = settings.insuranceRate;
double& processingFee = settings.processingFee;
bool& useInsurance = settings.useInsurance;

double& totalProcessed = loanStatistics.totalProcessed;
int& processedLoans = loanStatistics.processedLoans;

// Перетворення рядкового позначення на тип кредиту
bool parseLoanType(const string& value, LoanType& type) {
 if (value == "CONSUMER") {
 type = LoanType::Consumer;
 }
 else if (value == "MORTGAGE") {
 type = LoanType::Mortgage;
 }
 else if (value == "AUTO") {
 type = LoanType::Auto;
 }
 else {
 return false;
 }

 return true;
}

// Отримання текстової назви типу кредиту
string loanTypeToString(LoanType type) {
 switch (type) {
 case LoanType::Consumer:
 return "CONSUMER";

 case LoanType::Mortgage:
 return "MORTGAGE";

 case LoanType::Auto:
 return "AUTO";
 }

 return "UNKNOWN";
}

// Отримання параметрів відповідно до типу кредиту
bool getLoanRules(LoanType type, LoanRules& rules) {
 switch (type) {
 case LoanType::Consumer:
 rules = {
 0.02, 100000, 0.025,
 500, 20, 300,
 1.0, 60, 250,
 0.9, 200
 };
 break;

 case LoanType::Mortgage:
 rules = {
 0.01, 1000000, 0.015,
 1500, 15, 1000,
 0.8, 120, 750,
 0.95, 500
 };
 break;

 case LoanType::Auto:
 rules = {
 0.03, 500000, 0.035,
 800, 18, 500,
 1.2, 84, 400,
 0.85, 300
 };
 break;

 default:
 return false;
 }

 return true;
}

// Перевірка коректності вхідних даних
bool validateLoanInput(const LoanInput& input) {
 if (input.amount <= 0) {
 cout << "Помилка: сума кредиту повинна бути додатною." << endl;
 return false;
 }

 if (input.annualRate < 0 || input.annualRate > 100) {
 cout << "Помилка: некоректна відсоткова ставка." << endl;
 return false;
 }

 if (input.months <= 0 || input.months > 360) {
 cout << "Помилка: некоректний термін кредитування." << endl;
 return false;
 }

 return true;
}

// Розрахунок початкової комісії та додаткових зборів
void calculateLoanTypeFees(
 const LoanRules& rules,
 const LoanInput& input,
 LoanResult& result
) {
 result.commission = input.amount * rules.commissionRate;
 result.additionalFee = rules.baseAdditionalFee;

 if (input.annualRate > rules.rateThreshold) {
 result.additionalFee += rules.rateAdditionalFee;
 }

 if (input.amount > rules.amountThreshold) {
 result.commission = input.amount * rules.increasedCommissionRate;
 }
}

// Розрахунок щомісячного платежу
double calculateMonthlyPayment(
 double amount,
 double monthlyRate,
 int months
) {
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
double calculateInsurance(
 const LoanRules& rules,
 const LoanInput& input
) {
 if (input.insurance && settings.useInsurance) {
 return input.amount * settings.insuranceRate
 * rules.insuranceMultiplier;
 }

 return 0.0;
}

// Комісія за обслуговування терміну кредиту
void calculateServiceCommission(
 const LoanRules& rules,
 const LoanInput& input,
 LoanResult& result
) {
 result.commission += input.amount * settings.processingFee;

 if (input.months > rules.serviceMonthsThreshold) {
 result.commission += rules.serviceAdditionalFee;
 }
}

// Обробка дострокового погашення
void applyEarlyRepayment(
 const LoanRules& rules,
 const LoanInput& input,
 LoanResult& result
) {
 if (input.earlyRepayment) {
 result.totalInterest *= rules.earlyRepaymentFactor;
 result.additionalFee += rules.earlyRepaymentFee;
 }
}

// Виведення результатів розрахунку
void printLoanResult(
 const LoanInput& input,
 const LoanResult& result
) {
 if (result.totalAmount > input.amount * 2) {
 cout << "Увага: загальна вартість кредиту перевищує тіло кредиту вдвічі!" << endl;
 }

 if (result.insuranceCost > 0) {
 cout << "Страхування включено." << endl;
 }
 else {
 cout << "Страхування не включено." << endl;
 }

 if (input.earlyRepayment) {
 cout << "Застосовано опцію дострокового погашення." << endl;
 }
 else {
 cout << "Стандартні умови погашення." << endl;
 }

 cout << fixed << setprecision(2);
 cout << "\n----- РОЗРАХУНОК КРЕДИТУ -----" << endl;
 cout << "Тип кредиту: " << loanTypeToString(input.type) << endl;
 cout << "Тіло кредиту (сума): " << input.amount << endl;
 cout << "Щомісячний платіж: " << result.monthlyPayment << endl;
 cout << "Загальні відсотки: " << result.totalInterest << endl;
 cout << "Комісія: " << result.commission << endl;
 cout << "Страховка: " << result.insuranceCost << endl;
 cout << "Додаткові збори: " << result.additionalFee << endl;
 cout << "Загальна сума до сплати: " << result.totalAmount << endl;
}

// Основна типізована функція обробки кредиту
LoanResult processLoan(const LoanInput& input) {
 LoanResult result;

 if (!validateLoanInput(input)) {
 return result;
 }

 LoanRules rules;

 if (!getLoanRules(input.type, rules)) {
 cout << "Помилка: невідомий тип кредиту." << endl;
 return result;
 }

 double monthlyRate = input.annualRate / 12.0 / 100.0;

 calculateLoanTypeFees(rules, input, result);

 result.monthlyPayment = calculateMonthlyPayment(
 input.amount,
 monthlyRate,
 input.months
 );

 result.totalInterest =
 result.monthlyPayment * input.months - input.amount;

 result.insuranceCost = calculateInsurance(rules, input);

 calculateServiceCommission(rules, input, result);
 applyEarlyRepayment(rules, input, result);

 result.totalAmount =
 input.amount
 + result.totalInterest
 + result.commission
 + result.insuranceCost
 + result.additionalFee;

 result.success = true;

 loanStatistics.totalProcessed += result.totalAmount;
 loanStatistics.processedLoans++;

 printLoanResult(input, result);

 return result;
}

// Сумісна версія функції для попередніх регресійних тестів
double processLoan(
 string loanType,
 double amount,
 double annualRate,
 int months,
 bool earlyRepayment,
 bool insurance
) {
 LoanType type;

 if (!parseLoanType(loanType, type)) {
 cout << "Помилка: невідомий тип кредиту." << endl;
 return -1;
 }

 LoanInput input = {
 type,
 amount,
 annualRate,
 months,
 earlyRepayment,
 insurance
 };

 LoanResult result = processLoan(input);

 return result.success ? result.totalAmount : -1;
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

 bool earlyRepayment =
 (earlyChoice == 'y' || earlyChoice == 'Y');

 bool insurance =
 (insuranceChoice == 'y' || insuranceChoice == 'Y');

 LoanType loanType;

 if (!parseLoanType(type, loanType)) {
 cout << "Помилка: невідомий тип кредиту." << endl;
 cout << "\nПомилка розрахунку." << endl;
 }
 else {
 LoanInput input = {
 loanType,
 amount,
 rate,
 months,
 earlyRepayment,
 insurance
 };

 LoanResult result = processLoan(input);

 if (result.success) {
 cout << "\nРозрахунок завершено успішно." << endl;
 }
 else {
 cout << "\nПомилка розрахунку." << endl;
 }
 }

 cout << "\nОброблено кредитів: "
 << loanStatistics.processedLoans << endl;

 cout << "Загальна сума всіх оброблених кредитів: "
 << loanStatistics.totalProcessed << endl;

 return 0;
}