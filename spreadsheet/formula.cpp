
#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <set>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe)
{
    return output << fe.ToString();
}

namespace {
class Formula : public FormulaInterface {
public:
    explicit Formula(std::string expression)
        : ast_(ParseFormulaAST(std::move(expression))),
          referenced_cells_(ast_.GetCells().begin(), ast_.GetCells().end())
    {}

    Value Evaluate(const SheetInterface& sheet) const override
    {
        try {
            return ast_.Execute(sheet);
        } catch (const FormulaError e) {
            return e;
        }
    }

    // Используем "очищенную" формулу без лишних скобок из
    // FormulaAST::PrintFormula(std::ostream& out).
    std::string GetExpression() const override
    {
        std::stringstream ss;
        ast_.PrintFormula(ss);
        return ss.str();
    }

    std::vector<Position> GetReferencedCells() const override
    {
        // Вектор результатов (будет возвращен через NVRO)
        std::vector<Position> result;

        // Убираем дублирование ячеек, прогоняя их через std::set
        std::set<Position> unique_ref_cells(referenced_cells_.begin(), referenced_cells_.end());
        for (const auto& cell : unique_ref_cells)
        {
            result.push_back(cell);
        }
        return result;  //NVRO
    }


private:
    FormulaAST ast_;
    std::vector<Position> referenced_cells_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try
    {
        return std::make_unique<Formula>(std::move(expression));
    }
    catch (const std::exception&)
    {
        throw FormulaException("Formula parse error");
    }
}
