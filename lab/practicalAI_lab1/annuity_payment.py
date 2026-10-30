import math


def annuity_payment(
    principal: float,
    annual_rate: float,
    years: int,
    periods_per_year: int = 12,
) -> float:
    """Обчислює регулярний ануїтетний платіж за кредитом.

    Args:
        principal: Сума кредиту, більша за нуль.
        annual_rate: Річна процентна ставка у десятковому форматі.
        years: Строк кредитування в роках.
        periods_per_year: Кількість платежів на рік.

    Returns:
        Розмір одного регулярного платежу.

    Raises:
        ValueError: Якщо параметри некоректні або результат
        неможливо подати як скінченне число.
    """

    # Перевірка числових параметрів
    if (
        isinstance(principal, bool)
        or not isinstance(principal, (int, float))
        or not math.isfinite(principal)
        or principal <= 0
    ):
        raise ValueError(
            "Сума кредиту має бути скінченним числом, більшим за нуль."
        )

    if (
        isinstance(annual_rate, bool)
        or not isinstance(annual_rate, (int, float))
        or not math.isfinite(annual_rate)
        or annual_rate < 0
    ):
        raise ValueError(
            "Річна ставка має бути скінченним невід'ємним числом."
        )

    if (
        isinstance(years, bool)
        or not isinstance(years, int)
        or years <= 0
    ):
        raise ValueError(
            "Строк кредитування має бути додатним цілим числом."
        )

    if (
        isinstance(periods_per_year, bool)
        or not isinstance(periods_per_year, int)
        or periods_per_year <= 0
    ):
        raise ValueError(
            "Кількість платежів на рік має бути додатним цілим числом."
        )

    # Загальна кількість платежів і ставка за один період
    total_periods = years * periods_per_year
    period_rate = annual_rate / periods_per_year

    # Якщо відсоткова ставка дорівнює нулю
    if period_rate == 0:
        payment = principal / total_periods
    else:
        denominator = -math.expm1(
            -total_periods * math.log1p(period_rate)
        )
        payment = principal * period_rate / denominator

    if not math.isfinite(payment):
        raise ValueError(
            "Неможливо отримати скінченний результат розрахунку."
        )

    return payment


if __name__ == "__main__":
    result = annuity_payment(100_000.0, 0.06, 30)

    print(f"Щомісячний платіж: {result:.2f} грн")
    print(f"Загальна сума платежів: {result * 30 * 12:.2f} грн")
    print(
        "Загальна сума відсотків: "
        f"{result * 30 * 12 - 100_000:.2f} грн"
    )
