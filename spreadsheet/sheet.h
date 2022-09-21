#pragma once

#include "cell.h"
#include "common.h"

#include <unordered_map>

#include <functional>

class Sheet : public SheetInterface {
public:
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

    bool hasCircularReference(const Position& pos);
	// Можете дополнить ваш класс нужными полями и методами

private:
    std::unordered_map<Position, std::unique_ptr<Cell>, PositionHash> cells_;

    bool hasCircularReference(const Position& pos, std::unordered_set<Position, PositionHash>& visited_pos);
//    std::unordered_map<Cell*, std::vector<Position*>> cell_depends;
//    std::unordered_map<Cell*, std::vector<Position*>> cell_parents;

    Size size_;
};
