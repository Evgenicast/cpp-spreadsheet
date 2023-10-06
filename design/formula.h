#pragma once

#include "common.h"

#include <memory>
#include <deque>

class FormulaInterface
{
public:
    using Value = std::variant<double, FormulaError>;

    virtual ~FormulaInterface() = default;

    // Возвращает вычисленное значение формулы для переданного листа либо ошибку.
    // Если вычисление какой-то из указанных в формуле ячеек приводит к ошибке, то
    // возвращается именно эта ошибка. Если таких ошибок несколько, возвращается
    // любая.
    virtual Value Evaluate(const SheetInterface& sheet) const = 0;

    // Возвращает выражение, которое описывает формулу.
    // Не содержит пробелов и лишних скобок.
    virtual std::string GetExpression() const = 0;
	
	/*******новые методы******/
    // Возвращает список ячеек, которые используются при вычислении формулы.
    // Дано в подсказке к дизайн Ревью.
    virtual std::deque<Position> GetReferencedCells() const = 0;
};

// Парсит выражение и возвращает объект формулы.
// Бросает FormulaException, если формула синтаксически некорректна.
// Дается по умолчанию.
std::unique_ptr<FormulaInterface> ParseFormula(std::string expression);
