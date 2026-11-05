#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cmath>
#include <clocale> 
#include <windows.h>

#define main legacy_program_main
#include "../legacy/legacy_processor.cpp"
#undef main

using namespace std;

// Скидання глобальних змінних перед кожним тестом
void resetGlobals() {
    totalProcessed = 0.0;
    processedLoans = 0;
    useInsurance = true;
    insuranceRate = 0.025;
    processingFee = 0.015;
}

// Виклик функції з перехопленням консольного виводу
string captureOutput(
    const string& loanType,
    double amount,
    double annualRate,
    int months,
    bool earlyRepayment,
    bool insurance,
    double& result
) {
    ostringstream capturedOutput;
    streambuf* oldBuffer = cout.rdbuf(capturedOutput.rdbuf());

    result = processLoan(
        loanType,
        amount,
        annualRate,
        months,
        earlyRepayment,
        insurance
    );

    cout.rdbuf(oldBuffer);

    return capturedOutput.str();
}

// Перевірка одного тестового випадку
bool runTest(
    const string& id,
    const string& loanType,
    double amount,
    double rate,
    int months,
    bool earlyRepayment,
    bool insurance,
    double expectedResult,
    int expectedProcessedLoans
) {
    resetGlobals();

    double actualResult = 0.0;

    string output = captureOutput(
        loanType,
        amount,
        rate,
        months,
        earlyRepayment,
        insurance,
        actualResult
    );

    bool resultCorrect = abs(actualResult - expectedResult) < 0.01;
    bool counterCorrect = processedLoans == expectedProcessedLoans;
    
    // Якщо очікуваний результат від'ємний (-1.00), глобальна сума залишається 0.00
    double expectedTotal = (expectedResult < 0) ? 0.00 : expectedResult;
    bool totalCorrect = abs(totalProcessed - expectedTotal) < 0.01;

    bool testPassed = resultCorrect && counterCorrect && totalCorrect;

    cout << id << " | "
         << (testPassed ? "PASSED" : "FAILED")
         << " | actual = " << actualResult
         << " | expected = " << expectedResult
         << " | processedLoans = " << processedLoans
         << '\n';

    if (!testPassed) {
        cout << " Помилка перевірки:";

        if (!resultCorrect)
            cout << " результат";

        if (!counterCorrect)
            cout << " кількість оброблених кредитів";

        if (!totalCorrect)
            cout << " загальна сума";

        cout << '\n';
    }

    return testPassed;
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    setlocale(LC_ALL, "Ukrainian_Ukraine.65001");

    cout << fixed << setprecision(2);

    cout << "=== РЕГРЕСІЙНЕ ТЕСТУВАННЯ ===\n\n";

    int passed = 0;
    int failed = 0;

    // Коректні вхідні дані
    if (runTest("TC01", "CONSUMER", 50000,   18,  24,  false, false, 62158.92,   1)) passed++; else failed++;
    if (runTest("TC02", "MORTGAGE", 800000,  12,  120, false, false, 1398821.10, 1)) passed++; else failed++;
    if (runTest("TC03", "AUTO",     300000,  16,  60,  false, false, 452025.03,  1)) passed++; else failed++;
    if (runTest("TC04", "CONSUMER", 12000,    0,  12,  false, false, 12920.00,   1)) passed++; else failed++;
    if (runTest("TC05", "CONSUMER", 50000,   18,  24,  false, true,  63408.92,   1)) passed++; else failed++;
    if (runTest("TC06", "MORTGAGE", 800000,  12,  120, true,  true,  1386455.05, 1)) passed++; else failed++;
    if (runTest("TC07", "AUTO",     300000,  16,  60,  true,  false, 431666.27,  1)) passed++; else failed++;
    if (runTest("TC08", "CONSUMER", 10000,   10,  1,   false, false, 10933.33,   1)) passed++; else failed++;
    if (runTest("TC09", "MORTGAGE", 500000,  10,  360, false, false, 1594378.83, 1)) passed++; else failed++;

    // Некоректні вхідні дані
    if (runTest("TC10", "CONSUMER", 0,       10,  12,  false, false, -1.00,       0)) passed++; else failed++;
    if (runTest("TC11", "CONSUMER", -10000,  10,  12,  false, false, -1.00,       0)) passed++; else failed++;
    if (runTest("TC12", "CONSUMER", 10000,   -1,  12,  false, false, -1.00,       0)) passed++; else failed++;
    if (runTest("TC13", "CONSUMER", 10000,   101, 12,  false, false, -1.00,       0)) passed++; else failed++;
    if (runTest("TC14", "CONSUMER", 10000,   10,  0,   false, false, -1.00,       0)) passed++; else failed++;
    if (runTest("TC15", "CONSUMER", 10000,   10,  361, false, false, -1.00,       0)) passed++; else failed++;
    if (runTest("TC16", "UNKNOWN",  10000,   10,  12,  false, false, -1.00,       0)) passed++; else failed++;

    cout << "\n=== ПІДСУМКИ ТЕСТУВАННЯ ===\n";
    cout << "Усього тестів: " << passed + failed << '\n';
    cout << "Успішно: " << passed << '\n';
    cout << "Не пройдено: " << failed << '\n';

    if (failed == 0)
        cout << "\nУсі регресійні тести пройдено успішно!\n";
    else
        cout << "\nВиявлено помилки під час тестування!\n";

    return failed == 0 ? 0 : 1;
}
