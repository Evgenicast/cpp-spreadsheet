#pragma once

#include "cell.h"
#include "common.h"
#include <unordered_map>
#include <functional>
#include <set>

struct PositionHasher
{
    int operator()(const Position & Pos) const
    {
        return (Pos.col) + (Pos.row) * 16384;
    }
};

class Sheet : public SheetInterface
{
private:
    //std::vector<std::vector<Cell*>> m_SheetVector2D;// нужно писать метод инициализации, лучше мапа
    std::unordered_map<Position, Cell*, PositionHasher> m_SheetUnMap; // по заданию нужна асимптотика О(1) - эта хэштаблица
                                                                      // в предыдущем задании применено успешно.
    std::unordered_map<Position, std::set<Position>, PositionHasher> m_CellToDependeciesUMap; // ключ = ячейка, множество = зависимые от ключа ячейки

public:
    Sheet() = default;
    ~Sheet();

    Sheet(const Sheet &) = delete;
    Sheet & operator=(const Sheet &) = delete;
    Sheet(Sheet &&) noexcept = delete;
    Sheet & operator=(Sheet &&) noexcept = delete;

    void SetCell(Position Pos, std::string Text) override;

    const CellInterface * GetCell(Position pos) const override { return const_cast<Sheet*>(this)->GetCell(pos) ;}
    CellInterface * GetCell(Position pos) override;

    void ClearCell(Position pos) override;
    Size GetPrintableSize() const override;

    void PrintValues(std::ostream & output) const override;
    void PrintTexts(std::ostream & output) const override;

    /*******новые методы******/
    void InvalidateCell(const Position & Pos);
    void AddDependentCell(const Position& CurrSell, const Position & Pos) { m_CellToDependeciesUMap[CurrSell].insert(Pos); }
    const std::set<Position> GetDependentCells(const Position & Pos);
    void DeleteDependencies(const Position & Pos) { m_CellToDependeciesUMap.erase(Pos); };
};
