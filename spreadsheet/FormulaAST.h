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
    explicit FormulaAST(std::unique_ptr<ASTImpl::Expr> root_expr,
                        std::forward_list<Position> cell); // перемещается.
    FormulaAST(FormulaAST&&) = default;
    FormulaAST& operator=(FormulaAST&&) = default;
    ~FormulaAST();

    double Execute(FunctionPtr & func_Expression) const; // предлагается передавать функциональный объект(лямбду) по теории к Дизайн Ревью
    void PrintCells(std::ostream & out) const;
    void Print(std::ostream & out) const;
    void PrintFormula(std::ostream & out) const;

    /*******новые методы******/
    //Дано в подсказке в теории к Дизайн Ревью
    std::forward_list<Position> & GetCells() { return cells_; }
    const std::forward_list<Position>& GetCells() const { return cells_; }

private:
    std::unique_ptr<ASTImpl::Expr> root_expr_;
    std::forward_list<Position> cells_;

};

FormulaAST ParseFormulaAST(std::istream & in);
FormulaAST ParseFormulaAST(const std::string & in_str);
