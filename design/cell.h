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
    Impl * m_ImplPtr = nullptr; // принципиально в разработке своих классах использую сырой указатель. 
								// Не вижу смысла в юник. по 2 причинам: все равно приходится передавать сырой указатель, 
								//а это, в свою очередь, предполагает использование shareedPTR, который тем более стараюсь не использовать.
	
	SheetInterface & m_Sheet;   //ячейка должна хранить ссылку на Лист, т.к. его функционал позволяет вычилсять перекресные ссылки и не только. 
	
public:
    Cell(SheetInterface & Sheet); // как вариант можно создавать ячейку сразу передавая строковое значение (Cell(SheetInterface & Sheet, string text))
							      // но тогда придется использоват Set внутри конструктора. Пока не знаю, как лучше. Не сторонник использования методов внутри конструктора
								  // ячейка создается в листе как и прежде.
    ~Cell();
	
	// определяем все конструкторы и ассайменты явно, особенно, когда очевидно, что коприрование по дефолту может привести к undefined behaviour и другим ошибкам
    Cell(const Cell &) = delete;
    Cell & operator=(const Cell &) = delete;
    Cell(Cell&&) noexcept = delete;
    Cell & operator=(Cell &&) noexcept = delete;

    void Set(std::string text) override;
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
	
	/*******новые методы******/
	std::vector<Position> GetReferencedCells() const override; // объявленна в formula.h class FormulaInterface. Переопределан для каждого типа ячейки. 
															  // В новой реализации имеем дело не с деровом АСТ непсоредственно, а с кэшем ячеек в виде односвязного списка. 
															  // видимо (нет никаких тестов или задания), предполагается использовать данные списка для составления базы зависимых ячеек
															// при создании их в листе (sheet). Можно, наверное и тут сделать, но интуиция подсказывает, что базу лучше делать в листе.	
	
    bool IsCellCyclicDependent(const Cell * CurrSell, const Position & Pos) const; // Метод проверяет циклическую зависимость CurrSell от Pos в листе.Тут проверить невозможно при данной сигнатуре.
	/*NB. вроде в подсказках есть намеки на проверку валдиности кэша, но не вижу пока смысла в нем. Разве что для формулы.
	Должен быть вебинар на эту тему.
	*/

private:

    class Impl
    {
    public:
        virtual ~Impl() = default;
        virtual CellType GetType() const = 0;
        virtual CellInterface::Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
		
		/*******новые методы******/
		virtual std::deque<Position> GetReferencedCells() const = 0; // Список ячеек, от которых зависит текущая       		
    };

    class EmptyImpl : public Impl
    {
    public:
        EmptyImpl() = default;
        CellType GetType() const override;
        CellInterface::Value GetValue() const override;    
        std::string GetText() const override;              
		
		/*******новые методы******/
		std::deque<Position> GetReferencedCells() const = 0; 
    };

    class TextImpl : public Impl
    {
    public:
        explicit TextImpl(std::string Text);
        CellType GetType() const override;
        CellInterface::Value GetValue() const override;   
        std::string GetText() const override;   

		/*******новые методы******/
		std::deque<Position> GetReferencedCells() const = 0;         		

    private:
        std::string m_CellText;
        bool m_EscapeApostr = false;  // Экранировано ли содержимое esc-символом (апострофом)
    };

    class FormulaImpl : public Impl
    {
    public:
        FormulaImpl(SheetInterface & Sheet, std::string Formula);
        CellType GetType() const override;
        CellInterface::Value IGetValue() const override;    
        std::string GetText() const override;              
		
		/*******новые методы******/
		std::deque<Position> GetReferencedCells() const = 0;          	

    private:
        FormulaInterface * m_FormulaPtr = nullptr; // для форумальной ячейки нужен свой указатель и своя ссылка на таблицу. Как сделать все в одном интерфейсе не понимаю, но, видимо, и не нужно. 
        SheetInterface & m_Sheet; 
		CellInterface::Value> m_ExpressionValue; // кэшим значения вычисления формульной ячейки. 
    };
};