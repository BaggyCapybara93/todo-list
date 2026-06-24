#pragma once
#include <iostream>

    enum class Color {
        Default = 39,
        Red = 31,
        Green = 32,
        Yellow = 33,
        Blue = 34,
        Magenta = 35,
        Cyan = 36,
        White = 97
    };

class UI {
    private:

        std::string ansiColor(Color c) const {
            return "\033[" + std::to_string(static_cast<int>(c)) + "m";
        }

        std::string ansiBold(bool on) const {
            return on ? "\033[1m" : "\033[22m";
        }

        std::string ansiUnderline(bool on) const {
                return on ? "\033[4m" : "\033[24m";
        }

    public:
        static UI& instance() {
            static UI inst;
            return inst;
        }

        void print(const std::string& text, const Color& color = Color::White) const {
            std::cout
                << ansiColor(color)
                << text
                << "\033[0m"; // reset
        }

        void println(const std::string& text, const Color& color = Color::White) const {
            print(text, color);
            std::cout << "\n";
        }
};
