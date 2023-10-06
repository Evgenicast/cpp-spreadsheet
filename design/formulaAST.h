#pragma once

#include "FormulaLexer.h"
#include "common.h"

#include <forward_list>
#include <functional>
#include <stdexcept>

using FunctionPtr = const std::function<double(Position)>;

namespace ASTImpl
{
    class Expr;
}

class ParsingError : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class FormulaAST
{
public:
    explicit FormulaAST(std::unique_ptr<ASTImpl::Expr> root_expr
						std::forward_list<Position> СellsCellsFList); // перемещается. 
    FormulaAST(FormulaAST&&) = default;
    FormulaAST& operator=(FormulaAST&&) = default;
    ~FormulaAST();

    double Execute(FunctionPtr & func_Expression) const; // предлагается передавать функциональный объект(лямбду) по теории к Дизайн Ревью
    void Print(std::ostream& out) const;
    void PrintFormula(std::ostream& out) const;

    /*******новые методы******/
    //Дано в подсказе в теории к Дизайн Ревью
    std::forward_list<Position> & GetCells();
    const std::forward_list<Position>& GetCells() const;


private:
    std::unique_ptr<ASTImpl::Expr> root_expr_;
    std::forward_list<Position> m_CellsFList;

};

FormulaAST ParseFormulaAST(std::istream& in);
FormulaAST ParseFormulaAST(const std::string& in_str);
