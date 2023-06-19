#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <iostream>

using namespace std::literals;

std::ostream & operator<<(std::ostream & output, FormulaError fe)
{
    FormulaError::Category cat = fe.GetCategory();
    switch (cat)
    {
    case FormulaError::Category::Ref:
        return output << "#REF!";
    case FormulaError::Category::Value:
        return output << "#VALUE!";
    case FormulaError::Category::Div0:
        return output << "#DIV/0!";
    }
    return output << "";
}

namespace
{
    class Formula : public FormulaInterface
    {
    public:
        explicit Formula(std::string expression)
        : ast_(ParseFormulaAST(std::move(expression))), m_ReferencedCells(ast_.GetCells().cbegin(), ast_.GetCells().cend()){}
        Value Evaluate(const SheetInterface & Sheet) const override
        {
            try
            {
                return ast_.Execute([&Sheet](const Position & Pos)
                    {
                        if (Sheet.GetCell(Pos) == nullptr)
                        {
                            return 0.0;
                        }
                        auto VariantVal = Sheet.GetCell(Pos)->GetValue();

                        if (std::holds_alternative<double>(VariantVal))
                        {
                            return std::get<double>(VariantVal);
                        }
                        else if (std::holds_alternative<std::string>(VariantVal))
                        {
                            try
                            {
                                return std::stod(std::move(std::get<std::string>(VariantVal)));
                            }
                            catch (...)
                            {
                                throw FormulaError(FormulaError::Category::Value);
                            }
                        }
                        else
                        {
                            throw std::get<FormulaError>(VariantVal);
                        }
                        return 0.0;
                    });
            }
            catch (FormulaError & fe)
            {
                return fe;
            }
        }
        std::string GetExpression() const override
        {
            std::ostringstream out;
            ast_.PrintFormula(out);
            return out.str();
        }
        std::vector<Position> GetReferencedCells() const override
        {
            std::vector<Position> Result;
            std::set<Position> Positions(m_ReferencedCells.begin(), m_ReferencedCells.end());
            for (const auto & Pos : Positions)
            {
                Result.push_back(Pos);
            }
            return Result;
        }
    private:
        FormulaAST ast_;
        std::vector<Position> m_ReferencedCells;
    };
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression)
{
    try
    {
        return std::make_unique<Formula>(std::move(expression));
    }
    catch (const std::exception&)
    {
        throw FormulaException("ParseError");
    }
}
