#pragma once

#include "common.h"
#include "formula.h"
#include <optional>

enum class CellType
{
    EMPTY,
    TEXT,
    FORMULA,
    ERROR,
    DEPENDENT
};

class Cell : public CellInterface
{
private:
    class Impl;
    // принципиально в разработке своих классов использую сырой указатель.
    // Не вижу смысла в юник. по 2 причинам: все равно приходится передавать сырой указатель,
    // а это, в свою очередь, предполагает использование shareedPTR, который тем более стараюсь не использовать.
    Impl * m_CellValuePtr = nullptr;
    SheetInterface & m_Sheet;  // ячейка должна хранить ссылку на Лист, т.к. его функционал позволяет вычилсять перекресные ссылки и не только.

public:
    Cell(SheetInterface & sheet, std::string Text);//cpp
    ~Cell(){ delete m_CellValuePtr; }

    // определяем все конструкторы и ассайменты явно, особенно, когда очевидно, что коприрование по дефолту может привести к undefined behaviour и другим ошибкам
    Cell(const Cell &) = delete;
    Cell & operator=(const Cell &) = delete;
    Cell(Cell &&) noexcept = delete;
    Cell & operator=(Cell &&) noexcept = delete;

    void Set(std::string text); //cpp
    void Clear(){ m_CellValuePtr = new EmptyImpl(); }

    Value GetValue() const override { return  m_CellValuePtr->GetValue(); }
    std::string GetText() const override { return m_CellValuePtr->GetText(); }

    /*******новые методы******/
    // объявленна в formula.h class FormulaInterface. Переопределан для каждого типа ячейки.
    // В новой реализации имеем дело не с деровом АСТ непсоредственно, а с кэшем ячеек в виде односвязного списка.
    // видимо (нет никаких тестов или задания), предполагается использовать данные списка для составления базы зависимых ячеек
    // при создании их в листе (sheet). Можно, наверное и тут сделать, но интуиция подсказывает, что базу лучше делать в листе.
    std::vector<Position> GetReferencedCells() const override { return  m_CellValuePtr->GetReferencedCells(); }
    bool IsCellCyclicDependent(const Cell * CellToCheck, const Position & Pos) const;//cpp // Метод проверяет циклическую зависимость CurrSell от Pos в листе.Тут проверить невозможно при данной сигнатуре.
    void InvalidateCache() {  m_CellValuePtr->ResetCache(); }
    CellType GetType() const { return m_CellValuePtr->GetType(); }

private:

    class Impl
    {
    public:
        virtual ~Impl() = default;
        virtual CellType GetType() const = 0;
        virtual CellInterface::Value GetValue() const = 0;
        virtual std::string GetText() const = 0;

        /*******новые методы******/
        virtual std::vector<Position> GetReferencedCells() const = 0; // Список ячеек, от которых зависит текущая
        virtual void ResetCache() { return; }
    };

    class EmptyImpl : public Impl
    {
    public:
        EmptyImpl() = default;
        CellType GetType() const override { return CellType::EMPTY; }
        CellInterface::Value GetValue() const override { return 0.0; }
        std::string GetText() const override { return {}; }

        /*******новые методы******/
        std::vector<Position> GetReferencedCells() const override { return {}; }
    };

    class TextImpl : public Impl
    {
    private:
        std::string m_CellText;
        bool m_EscapeApostr = false;  // Экранировано ли содержимое esc-символом (апострофом)

    public:
        explicit TextImpl(std::string Text); //cpp
        CellType GetType() const override { return CellType::TEXT; }
        CellInterface::Value GetValue() const override; //cpp
        std::string GetText() const override { return m_CellText; }

        /*******новые методы******/
        std::vector<Position> GetReferencedCells() const override { return {}; }
    };

    class FormulaImpl : public Impl
    {
    private:
        SheetInterface & m_Sheet;
        std::unique_ptr<FormulaInterface> m_FormulaUnPtr; // для форумальной ячейки нужен свой указатель и своя ссылка на таблицу.
                                                          //На сырой не могу заменить, ибо дан по умолчание в парсере юник
        mutable std::optional <CellInterface::Value> m_ExpressionValue; // кэшим значения вычисления формульной ячейки.// update (в вебинаре посоветовали использовать mutable)

    public:
        FormulaImpl(SheetInterface & Sheet, std::string Formula)
        : m_Sheet(Sheet), m_FormulaUnPtr(ParseFormula(Formula)){};

        CellType GetType() const override { return CellType::FORMULA; }
        CellInterface::Value GetValue() const override;//cpp
        std::string GetText() const override { return { FORMULA_SIGN + m_FormulaUnPtr.get()->GetExpression() };}

        /*******новые методы******/
        std::vector<Position> GetReferencedCells() const override { return m_FormulaUnPtr.get()->GetReferencedCells(); }
        void ResetCache() override { m_ExpressionValue.reset(); }
    };
};
