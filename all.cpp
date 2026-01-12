#include <vld.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <ctime>
#include <algorithm>
#include <iomanip>

// ------------------ Вспомогательные функции ------------------

bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int daysInMonth(int year, int month) {
    static const int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (month < 1 || month > 12) return 0;
    int d = days[month - 1];
    if (month == 2 && isLeapYear(year)) d = 29;
    return d;
}

bool isValidDate(int year, int month, int day) {
    if (year < 1 || month < 1 || month > 12 || day < 1) return false;
    return day <= daysInMonth(year, month);
}

std::tm parseDate(const std::string& dateStr) {
    if (dateStr.size() != 10) throw std::invalid_argument("Invalid date format");
    if (dateStr[4] != '-' || dateStr[7] != '-') throw std::invalid_argument("Invalid separators");

    std::string y = dateStr.substr(0, 4);
    std::string m = dateStr.substr(5, 2);
    std::string d = dateStr.substr(8, 2);

    std::string all = y + m + d;
    if (std::any_of(all.begin(), all.end(), [](char c) {
            return !std::isdigit(static_cast<unsigned char>(c));
        })) {
        throw std::invalid_argument("Non-digit in date");
    }

    int year = std::stoi(y);
    int month = std::stoi(m);
    int day = std::stoi(d);

    if (!isValidDate(year, month, day))
        throw std::invalid_argument("Invalid calendar date");

    std::tm tm{};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = 12;
    tm.tm_isdst = -1;

    // Проверка нормализации: преобразуем в time_t и обратно
    std::time_t t = std::mktime(&tm);
    if (t == -1) throw std::invalid_argument("mktime failed");

    std::tm normalized = *std::localtime(&t);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &normalized);
    if (std::string(buf) != dateStr)
        throw std::invalid_argument("Normalized date differs from input");

    return tm;
}

bool isValidPlate(const std::string& plate) {
    if (plate.empty()) return false;
    return std::all_of(plate.begin(), plate.end(), [](char c) {
        return std::isupper(static_cast<unsigned char>(c)) || std::isdigit(static_cast<unsigned char>(c));
    });
}

// ------------------ Класс DateRecord ------------------

struct DateRecord {
    std::time_t timestamp;
    std::string plate;

    static DateRecord New(const std::string& str) {
        std::istringstream iss(str);
        std::string datePart, platePart;
        if (!(iss >> datePart >> platePart)) {
            throw std::invalid_argument("Invalid format: expected 'YYYY-MM-DD PLATE'");
        }
        // Убедимся, что нет лишних частей
        std::string extra;
        if (iss >> extra) {
            throw std::invalid_argument("Too many tokens in input");
        }

        std::tm tm = parseDate(datePart);
        if (!isValidPlate(platePart)) {
            throw std::invalid_argument("Invalid plate format: must contain only uppercase letters/digits");
        }

        std::time_t t = std::mktime(&tm);
        return { t, platePart };
    }
};

// ------------------ Класс DateList ------------------

class DateList {
public:
    std::vector<DateRecord> records;

    void clear() {
        records.clear();
    }

    void extract(const std::vector<std::string>& carList) {
        clear();
        for (size_t i = 0; i < carList.size(); ++i) {
            try {
                DateRecord rec = DateRecord::New(carList[i]);
                records.push_back(rec);
            }
            catch (const std::exception& e) {
                std::cout << "Ошибка в строке " << (i + 1) << ": " << e.what() << "\n";
            }
        }
    }

    void sortByDate() {
        std::sort(records.begin(), records.end(),
            [](const DateRecord& a, const DateRecord& b) {
                return a.timestamp < b.timestamp;
            });
    }

    void sortByPlateLetter() {
        std::sort(records.begin(), records.end(),
            [](const DateRecord& a, const DateRecord& b) {
                return a.plate[0] < b.plate[0];
            });
    }

    void print() const {
        for (const auto& rec : records) {
            std::tm tm = *std::localtime(&rec.timestamp);
            char buf[11];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
            std::cout << buf << " " << rec.plate << "\n";
        }
    }
};

// ------------------ Тесты ------------------

void run_tests() {
    struct TestCase {
        std::string input;
        bool valid;
    };

    std::vector<TestCase> tests = {
        {"2001-11-12 A000BC", true},
        {"2000-02-30 K009VO", false}, // неверная дата
        {"2000-13-01 X123YZ", false}, // месяц > 12
        {"bad input", false},
        {"2005-01-01", false},        // нет номера
        {"2005-01-01 a000bc", false}, // маленькие буквы
        {"2005-01-01 A000BC", true},
        {"1999-12-31 Z999ZZ", true}
    };

    int passed = 0;
    int total = static_cast<int>(tests.size());

    for (size_t i = 0; i < tests.size(); ++i) {
        bool ok = false;
        try {
            DateRecord::New(tests[i].input);
            ok = true;
        }
        catch (...) {
            ok = false;
        }

        if (ok == tests[i].valid) {
            ++passed;
        }
        else {
            std::cout << "❌Тест " << (i + 1) << " провален для: " << tests[i].input << "\n";
        }
    }

    std::cout << "\n✅Пройдено тестов: " << passed << " / " << total << "\n";
}

// ------------------ Основная программа ------------------

int main(int argc, char* argv[]) {
    bool is_test_mode = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "test") {
            is_test_mode = true;
            break;
        }
    }

    if (is_test_mode) {
        run_tests();
        return 0;
    }

    std::vector<std::string> car_infos = {
        "2001-11-12 A000BC",
        "2004-03-10 E003EK",
        "2003-03-10 K009VO",
        "2005-03-10 K009VO",
        "2000-03-10 K009VO",
        "2000-03-15 K009VO",
        "2000-03-11 K009VO"
    };

    DateList list;
    list.extract(car_infos);

    std::cout << "Выберите способ сортировки:\n";
    std::cout << "1 - по дате\n";
    std::cout << "2 - по первой букве номерного знака\n";
    std::cout << "Ваш выбор: ";

    std::string choice;
    std::getline(std::cin, choice);

    if (choice == "1") {
        list.sortByDate();
    }
    else if (choice == "2") {
        list.sortByPlateLetter();
    }
    else {
        std::cout << "Неверный выбор. Используется сортировка по дате по умолчанию.\n";
        list.sortByDate();
    }

    list.print();

    return 0;
}