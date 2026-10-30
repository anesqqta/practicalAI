import os
from dotenv import load_dotenv
from google import genai

# Завантажуємо API-ключ із файлу .env
load_dotenv()

api_key = os.getenv("GEMINI_API_KEY")

if not api_key:
 raise ValueError(
 "API-ключ не знайдено. Перевір файл .env."
 )

# Створюємо клієнт Gemini
client = genai.Client(api_key=api_key)

# Надсилаємо тестовий запит
response = client.models.generate_content(
 model="gemini-3.5-flash-lite",
 contents="Відповідай українською: що таке кредитний калькулятор? Поясни двома реченнями.",
)

print("Відповідь Gemini:")
print(response.text)