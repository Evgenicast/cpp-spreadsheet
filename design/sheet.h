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

    //так же как и в Cell.h - там комментарии.
    Sheet(const Sheet &) = delete;
    Sheet & operator=(const Sheet &) = delete;
    Sheet(Sheet &&) noexcept = delete;
    Sheet & operator=(Sheet &&) noexcept = delete;

    void SetCell(Position pos, std::string text) override;

    const CellInterface * GetCell(Position pos) const override;
    CellInterface * GetCell(Position pos) override;

    void ClearCell(Position pos) override;
    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

    /*******новые методы******/

    // Очищаем кэш для указанной ячейки и всех зависящих от нее
    // Пока точно не знаю, как сделать, но предполагаю, что если есть существующая ячейка
    // в методе SetCell нужено будет инвалидировать данные и зависимости.

    void InvalidateCell(const Position & Pos);

    // Формируем базу "Ключ - ячейка", "Множество - зависящие ячейки"
    void AddDependentCell(const Position& CurrSell, const Position & Pos);

    // Возвращает перечень ячеек, зависящих от pos
    const std::set<Position> GetDependentCells(const Position & Pos);

    // Удаляет зависимости для ячейки по pos.
    void DeleteDependencies(const Position & Pos);
};
