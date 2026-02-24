# 🎲 Kostka

## 📖 O projekcie
**Kostka** to projekt oparty na mikrokontrolerze z rodziny **ESP32**, stworzony przy użyciu środowiska ESP-IDF i napisany w języku C++. Głównym założeniem projektu jest sterowanie i obsługa matrycowych wyświetlaczy (opartych m.in. na układach MAX7219). Projekt łączy w sobie obsługę sprzętową, renderowanie grafiki oraz komunikację bezprzewodową.

Obecnie projekt znajduje się w fazie intensywnego rozwoju. Poniżej znajduje się lista planowanych zmian i optymalizacji.

## 🚀 Technologie
* **Mikrokontroler:** ESP32
* **Framework:** ESP-IDF (C/C++)
* **Narzędzia:** CMake
* **Sprzęt:** Wyświetlacze MAX7219, adxl345, l3g4200d

---

## 📝 TODO 

- [ ] **Dokończyć Bluetooth** – Ustabilizowanie i sfinalizowanie bezprzewodowej komunikacji z urządzeniem.
- [ ] **Zoptymalizować rysowanie** – Poprawa algorytmów renderujących, aby animacje i grafiki działały płynniej.
- [ ] **Optymalizacja dla wyświetlaczy MAX** – Napisać poprawki wydajnościowe stricte pod komunikację z wyświetlaczami MAX, aby zminimalizować opóźnienia i ewentualne migotanie (flickering).
- [ ] **Zmienić system przestawiania trybów** – Przebudować logikę przełączania się między trybami pracy / animacjami kostki.
- [ ] **Dodać klasowość (Refaktoryzacja OOP)** – Przepisać kod strukturalny na zorientowany obiektowo (C++), co ułatwi jego czytanie, utrzymanie i dalszą rozbudowę.

---
*Repozytorium w trakcie budowy. Zachęcam do śledzenia postępów!*