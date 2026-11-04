#include <iostream>
#include <string>
#include <iomanip>
#include <clocale> 
#include <windows.h>// Необхідно для підтримки української мови в консолі

using namespace std;

// Глобальні змінні
double insuranceRate = 0.025; // Базова ставка страхування
double processingFee = 0.015; // Комісія за обробку
double totalProcessed = 0.0;  // Загальна сума оброблених кредитів
int processedLoans = 0;       // Кількість оброблених кредитів
bool useInsurance = true;     // Чи використовувати страхування загалом

// Функція розрахунку параметрів кредиту
double processLoan(string loanType, double amount, double annualRate, int months, bool earlyRepayment, bool insurance) {
    double monthlyPayment = 0.0;
    double totalInterest = 0.0;
    double commission = 0.0;
    double insuranceCost = 0.0;
    double additionalFee = 0.0;
    double totalAmount = 0.0;
    double monthlyRate = annualRate / 12.0 / 100.0;

    // Валідація вхідних даних
    if (amount <= 0) {
        cout << "Помилка: сума кредиту повинна бути додатною." << endl;
        return -1;
    }

    if (annualRate < 0 || annualRate > 100) {
        cout << "Помилка: некоректна відсоткова ставка." << endl;
        return -1;
    }

    if (months <= 0 || months > 360) {
        cout << "Помилка: некоректний термін кредитування." << endl;
        return -1;
    }

    if (loanType != "CONSUMER" && loanType != "MORTGAGE" && loanType != "AUTO") {
        cout << "Помилка: невідомий тип кредиту." << endl;
        return -1;
    }

    // Обробка логіки залежно від типу кредиту
    if (loanType == "CONSUMER") {
        commission = amount * 0.02;
        additionalFee = 500;

        if (annualRate > 20) {
            additionalFee += 300;
        }

        if (amount > 100000) {
            commission = amount * 0.025;
        }
    } 
    else if (loanType == "MORTGAGE") {
        commission = amount * 0.01;
        additionalFee = 1500;

        if (annualRate > 15) {
            additionalFee += 1000;
        }

        if (amount > 1000000) {
            commission = amount * 0.015;
        }
    } 
    else if (loanType == "AUTO") {
        commission = amount * 0.03;
        additionalFee = 800;

        if (annualRate > 18) {
            additionalFee += 500;
        }

        if (amount > 500000) {
            commission = amount * 0.035;
        }
    }

    // Розрахунок щомісячного платежу
    if (monthlyRate == 0) {
        monthlyPayment = amount / months;
    } 
    else {
        double coefficient = monthlyRate * (1 + monthlyRate);
        double denominator = 1;

        for (int i = 0; i < months - 1; i++) {
            denominator *= (1 + monthlyRate);
        }

        double power = denominator * (1 + monthlyRate);
        monthlyPayment = amount * monthlyRate * power / (power - 1);
    }

    totalInterest = monthlyPayment * months - amount;

    // Розрахунок страховки
    if (insurance && useInsurance) {
        if (loanType == "CONSUMER") {
            insuranceCost = amount * insuranceRate;
        } 
        else if (loanType == "MORTGAGE") {
            insuranceCost = amount * insuranceRate * 0.8;
        } 
        else if (loanType == "AUTO") {
            insuranceCost = amount * insuranceRate * 1.2;
        }
    }

    // Розрахунок додаткових комісій за обслуговування терміну
    if (loanType == "CONSUMER") {
        commission += amount * processingFee;

        if (months > 60) {
            commission += 250;
        }
    } 
    else if (loanType == "MORTGAGE") {
        commission += amount * processingFee;

        if (months > 120) {
            commission += 750;
        }
    } 
    else if (loanType == "AUTO") {
        commission += amount * processingFee;

        if (months > 84) {
            commission += 400;
        }
    }

    // Обробка умов дострокового погашення
    if (earlyRepayment) {
        if (loanType == "CONSUMER") {
            totalInterest *= 0.9;
            additionalFee += 200;
        } 
        else if (loanType == "MORTGAGE") {
            totalInterest *= 0.95;
            additionalFee += 500;
        } 
        else if (loanType == "AUTO") {
            totalInterest *= 0.85;
            additionalFee += 300;
        }
    }

    // Розрахунок фінальної суми до сплати
    totalAmount = amount + totalInterest + commission + insuranceCost + additionalFee;

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

    // Оновлення глобальної статистики
    totalProcessed += totalAmount;
    processedLoans++;

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

    return totalAmount;
}

int main() {
    // Встановлення української локалі для коректного виведення в консоль
    // Повний запуск підтримки UTF-8 для Windows
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
