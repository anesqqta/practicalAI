import json
import os
import time
from datetime import datetime
from pathlib import Path

from dotenv import load_dotenv
from google import genai
from google.genai import types


# 1. Налаштування проєкту
BASE_DIR = Path(__file__).resolve().parent
load_dotenv(BASE_DIR / ".env")

API_KEY = os.getenv("GEMINI_API_KEY")
MODEL = "gemini-3.5-flash-lite"
TEMPERATURE = 0.2
REPEATS = 3

if not API_KEY:
    raise ValueError(
        "Не знайдено GEMINI_API_KEY. Перевір файл .env."
    )

client = genai.Client(api_key=API_KEY)


# 2. Однакові системні інструкції для всіх підходів
SYSTEM_PROMPT = """
Ти досвідчений Python-розробник.
Пиши зрозумілий і надійний код з анотаціями типів.
Перевіряй вхідні дані та крайові випадки.
Не використовуй сторонні бібліотеки для математичних розрахунків.
Поверни лише код Python без Markdown-огорож.
"""


# 3. Спільне завдання для всіх підходів
TASK = """
Реалізуй функцію для розрахунку щомісячного ануїтетного
платежу за кредитом.

Обов'язкова сигнатура:
annuity_payment(
    principal: float,
    annual_rate: float,
    years: int,
    periods_per_year: int = 12
) -> float

Вимоги:
1. principal — сума кредиту, більша за 0.
2. annual_rate — річна процентна ставка у десятковому
   форматі (наприклад, 0.06 означає 6%), не менша за 0.
3. years — додатне ціле число.
4. periods_per_year — додатне ціле число.
5. Для нульової ставки платіж дорівнює principal / n,
   де n — загальна кількість платежів.
6. Для додатної ставки використовуй формулу:
   payment = principal * r / (1 - (1 + r) ** (-n))
   де r = annual_rate / periods_per_year,
   n = years * periods_per_year.
7. Некоректні аргументи мають спричиняти ValueError.
8. Додай docstring та анотації типів.
9. Поверни повний код Python без Markdown-огорож
   та без тексту поза кодом.
"""


# 4. Промпти для трьох підходів
PROMPTS = {
    "Zero-Shot": TASK,
    "Few-Shot": (
        "Приклад оформлення функції:\n\n"
        "def add_rate(amount: float, rate: float) -> float:\n"
        '    """Обчислює суму з урахуванням ставки."""\n'
        "    if amount <= 0 or rate < 0:\n"
        '        raise ValueError("Некоректні аргументи.")\n'
        "    return amount * (1 + rate)\n\n"
        "Використай приклад як орієнтир для оформлення, "
        "але реалізуй саме розрахунок кредитного платежу.\n\n"
        + TASK
    ),
    "Chain-of-Thought": (
        "Працюй послідовно:\n"
        "1. Визнач кількість платежів і періодичну ставку.\n"
        "2. Перевір обмеження вхідних аргументів.\n"
        "3. Окремо опрацюй випадок нульової ставки.\n"
        "4. Для додатної ставки застосуй задану формулу.\n"
        "5. Перевір типи та крайові випадки.\n\n"
        "Не виводь проміжних міркувань. "
        "Поверни лише остаточний код Python.\n\n"
        + TASK
    ),
}


# 5. Підготовка файлу результатів
OUTPUT_DIR = BASE_DIR / "generations"
OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
LOG_FILE = OUTPUT_DIR / "experiment_log.json"

results = []


def save_results() -> None:
    """Зберігає поточні результати експерименту у JSON."""
    LOG_FILE.write_text(
        json.dumps(results, ensure_ascii=False, indent=2),
        encoding="utf-8",
    )


# 6. Виконання експерименту
print("Починаємо експеримент із промптами Gemini.")
print(f"Модель: {MODEL}")
print(f"Температура: {TEMPERATURE}")
print(f"Заплановано генерацій: {len(PROMPTS) * REPEATS}")
print()

for approach, prompt in PROMPTS.items():
    for iteration in range(1, REPEATS + 1):
        print(f"[{approach}] Генерація {iteration}/{REPEATS}")

        start_time = time.perf_counter()

        record = {
            "approach": approach,
            "iteration": iteration,
            "model": MODEL,
            "temperature": TEMPERATURE,
            "system_prompt": SYSTEM_PROMPT,
            "prompt": prompt,
            "output": "",
            "execution_time_seconds": None,
            "status": "error",
            "timestamp": datetime.now().isoformat(
                timespec="seconds"
            ),
        }

        try:
            response = client.models.generate_content(
                model=MODEL,
                contents=prompt,
                config=types.GenerateContentConfig(
                    system_instruction=SYSTEM_PROMPT,
                    temperature=TEMPERATURE,
                ),
            )

            output_text = response.text or ""
            record["output"] = output_text
            record["status"] = (
                "success" if output_text.strip() else "empty"
            )
            print(f"Отримано символів: {len(output_text)}")

        except Exception as error:
            record["error"] = str(error)
            print(f"Помилка API: {error}")

        finally:
            elapsed = time.perf_counter() - start_time
            record["execution_time_seconds"] = round(elapsed, 4)

        results.append(record)
        save_results()

        print(f"Час запиту: {record['execution_time_seconds']} с")
        print(f"Статус: {record['status']}")
        print()


# 7. Підсумок
successful = sum(
    1 for item in results if item["status"] == "success"
)
failed = len(results) - successful

print("Експеримент завершено.")
print(f"Усього спроб: {len(results)}")
print(f"Успішних генерацій: {successful}")
print(f"Неуспішних або порожніх відповідей: {failed}")
print(f"Файл результатів: {LOG_FILE}")
