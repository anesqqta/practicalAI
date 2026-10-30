import math
import time
from annuity_payment import annuity_payment


def run_tests():
    tests_passed = 0
    tests_failed = 0

    # Коректні вхідні дані
    valid_tests = [
        (
            "Стандартний кредит",
            (100_000.0, 0.06, 30),
            599.550525,
        ),
        (
            "Нульова ставка",
            (1_200.0, 0.0, 1),
            100.0,
        ),
        (
            "Інша періодичність",
            (1_000.0, 0.12, 2, 4),
            142.45638882723907,
        ),
    ]

    for name, args, expected in valid_tests:
        try:
            actual = annuity_payment(*args)

            if math.isclose(
                actual, expected, rel_tol=1e-5, abs_tol=1e-4
            ):
                print(f"{name}: OK")
                tests_passed += 1
            else:
                print(
                    f"{name}: ПОМИЛКА — "
                    f"очікувалося {expected}, отримано {actual}"
                )
                tests_failed += 1

        except Exception as error:
            print(
                f"{name}: ПОМИЛКА — {type(error).__name__}: {error}"
            )
            tests_failed += 1

    # Некоректні вхідні дані
    invalid_tests = [
        ("Нульова сума", (0.0, 0.06, 10)),
        ("Від'ємна сума", (-1000.0, 0.06, 10)),
        ("Від'ємна ставка", (1000.0, -0.01, 10)),
        ("Нульовий строк", (1000.0, 0.06, 0)),
        ("Дробовий строк", (1000.0, 0.06, 2.5)),
        ("Нуль періодів", (1000.0, 0.06, 10, 0)),
        ("Рядок замість суми", ("1000", 0.06, 10)),
        ("Булеве значення", (True, 0.06, 10)),
        ("Нескінченна сума", (math.inf, 0.06, 10)),
        ("NaN у ставці", (1000.0, math.nan, 10)),
        ("Нескінченна ставка", (1000.0, math.inf, 10)),
        ("Дробова кількість платежів", (1000.0, 0.06, 10, 2.5)),
    ]

    for name, args in invalid_tests:
        try:
            annuity_payment(*args)
            print(f"{name}: ПОМИЛКА — виняток не виник")
            tests_failed += 1

        except ValueError:
            print(f"{name}: OK — ValueError")
            tests_passed += 1

        except Exception as error:
            print(
                f"{name}: ПОМИЛКА — "
                f"неочікуваний {type(error).__name__}"
            )
            tests_failed += 1

    # Вимірювання часу виконання
    repeats = 100_000
    start = time.perf_counter()

    for _ in range(repeats):
        annuity_payment(100_000.0, 0.06, 30)

    average_time = (time.perf_counter() - start) / repeats

    print("\n========== ПІДСУМОК ==========")
    print(f"Успішно: {tests_passed}")
    print(f"Помилок: {tests_failed}")
    print(f"Усього тестів: {tests_passed + tests_failed}")
    print(f"Середній час виклику: {average_time:.9f} с")

    if tests_failed:
        raise SystemExit("Фінальний модуль не пройшов усі тести.")

    print("Фінальний модуль успешно пройшов усі тести.")


if __name__ == "__main__":
    run_tests()
