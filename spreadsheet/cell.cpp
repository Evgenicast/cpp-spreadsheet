#include "cell.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <optional>

Cell::Cell(SheetInterface & sheet, std::string Text)
:m_Sheet(sheet)
{
    m_CellValuePtr = new EmptyImpl();
    Set(Text);
}

void Cell::Set(std::string text)
{
    if (text.empty())
    {
        m_CellValuePtr = new EmptyImpl();
        return;
    }
    else if (text[0] != FORMULA_SIGN || (text[0] == FORMULA_SIGN && text.size() == 1))
    {
        m_CellValuePtr = new TextImpl(text);
        return;
    }
    else
    {
        std::string sub_str = text.erase(0, 1);

        try
        {
            m_CellValuePtr = new FormulaImpl(m_Sheet, sub_str);
        }
        catch (...)
        {
            std::string ExceptionRef = "#REF!";
            throw FormulaException(ExceptionRef);
        }
        return;
    }
}

bool Cell::IsCellCyclicDependent(const Cell * CellToCheck, const Position & Pos) const
{
    for (const auto & CellPos : GetReferencedCells())
    {
        const Cell * ReferCell = dynamic_cast<const Cell*>(m_Sheet.GetCell(CellPos));

        if (Pos == CellPos)
        {
            return true;
        }

        if (!ReferCell)
        {
            m_Sheet.SetCell(CellPos, "");
            ReferCell = dynamic_cast<const Cell*>(m_Sheet.GetCell(CellPos));
        }

        if (CellToCheck == ReferCell)
        {
            return true;
        }

        if (ReferCell->IsCellCyclicDependent(CellToCheck, Pos))
        {
            return true;
        }
    }
    return false;
}

Cell::TextImpl::TextImpl(std::string Text)
: m_CellText(std::move(Text))
{
    if (m_CellText[0] == ESCAPE_SIGN)
    {
        m_EscapeApostr = true;
    }
}

CellInterface::Value Cell::TextImpl::GetValue() const
{
    if (m_EscapeApostr)
    {
        // Возвращаем без апострофа
        return m_CellText.substr(1, m_CellText.size() - 1);
    }
    else
    {
        return m_CellText;
    }
}

CellInterface::Value Cell::FormulaImpl::GetValue() const
{
    if (!m_ExpressionValue)
    {
        for (const auto & Pos : m_FormulaUnPtr.get()->GetReferencedCells())
        {
            if (dynamic_cast<Cell*>(m_Sheet.GetCell(Pos))->GetType() == CellType::TEXT)
            {
                m_ExpressionValue = FormulaError(FormulaError::Category::Value);
                return *m_ExpressionValue;
            }
        }

        const auto VariantVal = m_FormulaUnPtr->Evaluate(m_Sheet);

        if (std::holds_alternative<double>(VariantVal))
        {
            double Result = std::get<double>(VariantVal);
            if (std::isinf(Result))
            {
                m_ExpressionValue = FormulaError(FormulaError::Category::Div0);
            }
            else
            {
                m_ExpressionValue = Result;
            }
        }
        else if (std::holds_alternative<FormulaError>(VariantVal))
        {
            m_ExpressionValue = std::get<FormulaError>(VariantVal);
        }
    }
    return *m_ExpressionValue;
}
