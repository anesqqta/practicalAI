#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <clocale>
#include <windows.h>

#define main legacy_program_main
#include "../legacy/legacy_processor.cpp"
#undef main

using namespace std;

// Виконання одного регресійного тесту
bool runTest(
    const string& testName,
    const LoanInput& input,
    double expectedAmount,
    int expectedProcessedLoans
) {
    // Окремий стан системи для кожного тесту
    LoanSystemState state;

    // Перехоплення повідомлень програми
    ostringstream output;
    streambuf* oldCout = cout.rdbuf(output.rdbuf());

    LoanResult result = processLoan(input, state);

    // Відновлення стандартного виведення
    cout.rdbuf(oldCout);

    double actualAmount = result.success ? result.totalAmount : -1.0;

    bool amountCorrect = fabs(actualAmount - expectedAmount) < 0.01;
    bool statisticsCorrect = state.statistics.processedLoans == expectedProcessedLoans;

    double expectedTotalProcessed = expectedProcessedLoans > 0 ? expectedAmount : 0.0;
    bool totalProcessedCorrect = fabs(state.statistics.totalProcessed - expectedTotalProcessed) < 0.01;

    bool testPassed = amountCorrect && statisticsCorrect && totalProcessedCorrect;

    cout << testName << ": ";

    if (testPassed) {
        cout << "PASSED";
    }
    else {
        cout << "FAILED"
             << " | Очікувана сума: " << expectedAmount
             << " | Фактична сума: " << actualAmount
             << " | Оброблено кредитів: " << state.statistics.processedLoans;
    }

    cout << endl;

    return testPassed;
}

// Перевірка відхилення невідомого типу кредиту
bool testUnknownLoanType() {
    LoanType type;

    bool result = parseLoanType("UNKNOWN", type);
    bool testPassed = !result;

    cout << "TC16: ";

    if (testPassed) {
        cout << "PASSED";
    }
    else {
        cout << "FAILED";
    }

    cout << endl;

    return testPassed;
}

int main() {

    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    setlocale(LC_ALL, "Ukrainian_Ukraine.65001");
    
    int totalTests = 16;
    int passedTests = 0;

    cout << "===== РЕГРЕСІЙНЕ ТЕСТУВАННЯ =====" << endl;

    // TC01: Споживчий кредит без дострокового погашення та страхування
    passedTests += runTest("TC01", {LoanType::Consumer, 50000, 18, 24, false, false}, 62158.92, 1);

    // TC02: Іпотечний кредит
    passedTests += runTest("TC02", {LoanType::Mortgage, 800000, 12, 120, false, false}, 1398821.10, 1);

    // TC03: Автокредит
    passedTests += runTest("TC03", {LoanType::Auto, 300000, 16, 60, false, false}, 452025.03, 1);

    // TC04: Кредит з нульовою відсотковою ставкою
    passedTests += runTest("TC04", {LoanType::Consumer, 12000, 0, 12, false, false}, 12920.00, 1);

    // TC05: Споживчий кредит зі страхуванням
    passedTests += runTest("TC05", {LoanType::Consumer, 50000, 18, 24, false, true}, 63408.92, 1);

    // TC06: Іпотечний кредит з достроковим погашенням і страхуванням
    passedTests += runTest("TC06", {LoanType::Mortgage, 800000, 12, 120, true, true}, 1386455.05, 1);

    // TC07: Автокредит з достроковим погашенням
    passedTests += runTest("TC07", {LoanType::Auto, 300000, 16, 60, true, false}, 431666.27, 1);

    // TC08: Кредит на один місяць
    passedTests += runTest("TC08", {LoanType::Consumer, 10000, 10, 1, false, false}, 10933.33, 1);

    // TC09: Кредит на максимальний термін
    passedTests += runTest("TC09", {LoanType::Mortgage, 500000, 10, 360, false, false}, 1594378.83, 1);

    // TC10: Нульова сума кредиту
    passedTests += runTest("TC10", {LoanType::Consumer, 0, 10, 12, false, false}, -1.0, 0);

    // TC11: Від'ємна сума кредиту
    passedTests += runTest("TC11", {LoanType::Consumer, -10000, 10, 12, false, false}, -1.0, 0);

    // TC12: Від'ємна відсоткова ставка
    passedTests += runTest("TC12", {LoanType::Consumer, 10000, -1, 12, false, false}, -1.0, 0);

    // TC13: Відсоткова ставка понад 100%
    passedTests += runTest("TC13", {LoanType::Consumer, 10000, 101, 12, false, false}, -1.0, 0);

    // TC14: Нульовий термін кредитування
    passedTests += runTest("TC14", {LoanType::Consumer, 10000, 10, 0, false, false}, -1.0, 0);

    // TC15: Термін кредитування понад 360 місяців
    passedTests += runTest("TC15", {LoanType::Consumer, 10000, 10, 361, false, false}, -1.0, 0);

    // TC16: Невідомий тип кредиту
    passedTests += testUnknownLoanType();

    cout << "\n===== ПІДСУМКИ ТЕСТУВАННЯ =====" << endl;
    cout << "Усього тестів: " << totalTests << endl;
    cout << "Успішно: " << passedTests << endl;
    cout << "Не пройдено: " << totalTests - passedTests << endl;

    if (passedTests == totalTests) {
        cout << "\nУсі регресійні тести пройдено успішно!" << endl;
        return 0;
    }

    cout << "\nВиявлено помилки під час тестування!" << endl;
    return 1;
}
