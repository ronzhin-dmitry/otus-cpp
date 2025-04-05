#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

std::vector<std::string> parse_csv_line(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    bool in_quotes = false;
    char prev_char = 0;

    for (char c : line) {
        if (c == '"') {
            // Обработка кавычек
            if (in_quotes && prev_char == '"') {
                token += '"'; // Экранированные кавычки ("")
            }
            in_quotes = !in_quotes;
        } else if (c == ',' && !in_quotes) {
            // Разделитель полей (только вне кавычек)
            tokens.push_back(token);
            token.clear();
        } else {
            token += c;
        }
        prev_char = c;
    }
    tokens.push_back(token); // Добавляем последнее поле

    // Удаляем окружающие кавычки (если есть)
    for (auto& t : tokens) {
        if (!t.empty() && t.front() == '"' && t.back() == '"') {
            t = t.substr(1, t.size() - 2);
        }
    }

    return tokens;
}

int main() {
    std::string line;
    while (std::getline(std::cin, line)) {
        std::vector<std::string> parsed = parse_csv_line(line);
        
        if (parsed.size() < 10) {
            continue; // Пропускаем некорректные строки
        }

        try {
            int price = std::stoi(parsed[9]);
            if (price < 0) continue; // Игнорировать некорректные цены
            std::cout << price << "," << price * price << std::endl;
        } catch (...) {
            continue;
        }
    }
    return 0;
}