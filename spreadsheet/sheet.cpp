#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet()
{
    for (auto & [key, CellPtr] : m_SheetUnMap)
    {
        delete CellPtr;
    }
}

void Sheet::SetCell(Position Pos, std::string Text)
{
    if (!Pos.IsValid())
    {
        throw InvalidPositionException("Invalid position (SetCell error)");
    }

    Cell * ValidCellPtr = dynamic_cast<Cell*>(GetCell(Pos)); // пытаемся записать ячейку в переменную  сушестрвующая_ячейка

    if (ValidCellPtr && ValidCellPtr->GetText() == Text) // если она присутствоует в таблице и ее содержимое рвыно аргументу
    {
        return; // завершаем работу метода
    }

    if (ValidCellPtr) // если она присутствоует в таблице
    {
        std::string OldText = ValidCellPtr->GetText(); // записываем старое содержимое ячейки в переменную старый_текс
        InvalidateCell(Pos);
        DeleteDependencies(Pos);
        ValidCellPtr->Set(std::move(Text)); // записываем в ячейку новое содержимое
        if (ValidCellPtr->IsCellCyclicDependent(ValidCellPtr, Pos)) // если есть цикличиская зависимость
        {
            ValidCellPtr->Set(std::move(OldText)); // возвращаем старое содержимое обратно
            throw CircularDependencyException("Circular Exception!"); // кидаем исключение
        }

        for (const auto & ReferPos : ValidCellPtr->GetReferencedCells())
        {
            AddDependentCell(ReferPos, Pos);
        }
    }
    else // если ячейки нет
    {
        Cell * NewCell = new Cell(*this, Text); // зоздаем

        if (NewCell->IsCellCyclicDependent(NewCell, Pos)) // проверяем на циклическую завистимость, если есть
        {
            throw CircularDependencyException("Circular Exception!"); // кидаем тров, завершаем метод
        }

        for (const auto & ReferPos : NewCell->GetReferencedCells())
        {
            AddDependentCell(ReferPos, Pos);
        }
        m_SheetUnMap[Pos] = std::move(NewCell); // записвыдваем в таблицу
    }
}

CellInterface * Sheet::GetCell(Position pos)
{
    if(!pos.IsValid())
    {
        throw InvalidPositionException("Invalid position (GetCell error)");
    }

    try
    {
        return m_SheetUnMap.at(pos);
    }
    catch(...)
    {
        return nullptr;
    }
}

void Sheet::ClearCell(Position pos)
{
    if (!pos.IsValid())
    {
        throw InvalidPositionException("Invalid position (ClearCell error)");
    }

    if (pos.IsValid() && m_SheetUnMap.find(pos) != m_SheetUnMap.end())
    {
        m_SheetUnMap.erase(pos);
    }
}

Size Sheet::GetPrintableSize() const
{
    Size s;
    for (const auto & [pos, _] : m_SheetUnMap)
    {
        s.cols = std::max(s.cols, pos.col + 1); // защита от отрицательного значения
        s.rows = std::max(s.rows, pos.row + 1);
    }
    return s;
}

void Sheet::PrintValues(std::ostream& output) const
{
    auto Size = GetPrintableSize();
    for (int i = 0; i < Size.rows; ++i)
    {
        bool IsTabulationNeeded = false;
        for (int j = 0; j < Size.cols; ++j)
        {
            if (IsTabulationNeeded)
            {
                output << '\t';
            }
            IsTabulationNeeded = true;
            if (auto cell = GetCell({ i, j }); cell)
            {
                auto val = cell->GetValue();
                if (std::holds_alternative<std::string>(val))
                {
                    output << std::get<std::string>(val);
                }
                if (std::holds_alternative<double>(val))
                {
                    output << std::get<double>(val);
                }
                if (std::holds_alternative<FormulaError>(val))
                {
                    output << std::get<FormulaError>(val);
                }
            }
        }
        output << '\n';
    }
}

void Sheet::PrintTexts(std::ostream & output) const
{
    auto size = GetPrintableSize();
    for (int i = 0; i < size.rows; ++i)
    {
        bool IsTabulationNeeded = false;
        for (int j = 0; j < size.cols; ++j)
        {
            if (IsTabulationNeeded)
            {
                output << '\t';
            }
            IsTabulationNeeded = true;
            if (auto cell = GetCell({ i, j }); cell)
            {
                output << cell->GetText();
            }
        }
        output << '\n';
    }
}

void Sheet::InvalidateCell(const Position & Pos)
{
    for (const auto cell_pos : GetDependentCells(Pos))
    {
        auto cell = GetCell(cell_pos);
        dynamic_cast<Cell*>(cell)->InvalidateCache();
        InvalidateCell(cell_pos);
    }
}

const std::set<Position> Sheet::GetDependentCells(const Position & Pos)
{
    try
    {
        return m_CellToDependeciesUMap.at(Pos);
    }
    catch (...)
    {
        return {};
    }
}

std::unique_ptr<SheetInterface> CreateSheet()
{
    return std::make_unique<Sheet>();
}
