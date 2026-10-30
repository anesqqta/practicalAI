import json
import math
import time
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parent
LOG_FILE = BASE_DIR / "generations" / "experiment_log.json"
REPEATS = 10_000

with LOG_FILE.open("r", encoding="utf-8") as file:
    records = json.load(file)

tests = [
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

invalid_tests = [
    ("Нульова сума", (0.0, 0.06, 10)),
    ("Від'ємна ставка", (1000.0, -0.01, 10)),
    ("Нульовий строк", (1000.0, 0.06, 0)),
    ("Нуль періодів", (1000.0, 0.06, 10, 0)),
    ("Рядок замість суми", ("1000", 0.06, 10)),
    ("Дробовий строк", (1000.0, 0.06, 2.5)),
]

summary = []

for record in records:
    label = f"{record['approach']} #{record['iteration']}"
    namespace = {}
    passed = 0
    failed = 0
    errors = []

    try:
        exec(
            compile(record["output"], label, "exec"),
            namespace,
        )

        function = namespace.get("annuity_payment")
        if not callable(function):
            raise RuntimeError(
                "Функцію annuity_payment не знайдено."
            )

    except Exception as error:
        print(f"\n{label}: ПОМИЛКА ЗАПУСКУ — {error}")
        summary.append({
            "variant": label,
            "status": "runtime error",
            "passed": 0,
            "failed": len(tests) + len(invalid_tests),
            "average_call_time_seconds": None,
        })
        continue

    for name, args, expected in tests:
        try:
            actual = function(*args)
            if (
                isinstance(actual, (int, float))
                and math.isfinite(actual)
                and math.isclose(
                    actual, expected, rel_tol=1e-5, abs_tol=1e-4
                )
            ):
                passed += 1
                result = "OK"
            else:
                failed += 1
                result = f"ПОМИЛКА: отримано {actual!r}"

        except Exception as error:
            failed += 1
            result = f"ПОМИЛКА: {type(error).__name__}: {error}"

        print(f"{label} | {name}: {result}")

    for name, args in invalid_tests:
        try:
            function(*args)
            failed += 1
            result = "ПОМИЛКА: виняток не виник"
        except ValueError:
            passed += 1
            result = "OK: ValueError"
        except Exception as error:
            failed += 1
            result = f"ПОМИЛКА: {type(error).__name__}"

        print(f"{label} | {name}: {result}")

    try:
        start = time.perf_counter()
        for _ in range(REPEATS):
            function(100_000.0, 0.06, 30)
        average_time = (time.perf_counter() - start) / REPEATS
    except Exception as error:
        average_time = None
        errors.append(str(error))

    summary.append({
        "variant": label,
        "status": "tested",
        "passed": passed,
        "failed": failed,
        "average_call_time_seconds": average_time,
        "errors": errors,
    })

print("\n========== ПІДСУМОК ==========")
for item in summary:
    print(
        f"{item['variant']}: "
        f"успішно {item['passed']}, "
        f"помилок {item['failed']}, "
        f"середній час виклику "
        f"{item['average_call_time_seconds']}"
    )

result_file = BASE_DIR / "generations" / "evaluation_results.json"
result_file.write_text(
    json.dumps(summary, ensure_ascii=False, indent=2),
    encoding="utf-8",
)
print(f"\nРезультати збережено: {result_file}")
