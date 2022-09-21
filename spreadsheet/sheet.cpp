#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <unordered_set>
#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

void CheckPositin(const Position& position) {
    if (!position.IsValid()){
        throw InvalidPositionException("Invalid position");
    }
}

Sheet::~Sheet() {

}

void Sheet::SetCell(Position pos, std::string text) {
    CheckPositin(pos);

    if (!cells_.count(pos)){
        cells_[pos] = std::make_unique<Cell>(*this);

        cells_[pos]->Set(std::move(text));

        if (hasCircularReference(pos)){
            cells_[pos]->Clear();
            throw CircularDependencyException("Circular dependency" );
        }

        size_.rows = std::max(pos.row + 1, size_.rows);
        size_.cols = std::max(pos.col + 1, size_.cols);
    } else {
        std::string old_text = cells_[pos]->GetText();

        cells_[pos]->Set(std::move(text));

        if (hasCircularReference(pos)){
            cells_[pos]->Set(std::move(old_text));
            throw CircularDependencyException("Circular dependency" );
        }

    }

    for (const auto& cell: cells_[pos]->GetReferencedCells()) {
        if (!GetCell(cell))
            SetCell(cell, std::string{});
    }

}

const CellInterface* Sheet::GetCell(Position pos) const {
    CheckPositin(pos);
    if (cells_.count(pos)){
        return cells_.at(pos).get();
    } else {
        return nullptr;
    }
}

CellInterface* Sheet::GetCell(Position pos) {
    CheckPositin(pos);
    if (cells_.count(pos)){
        return cells_.at(pos).get();
    } else {
        return nullptr;
    }
}

void Sheet::ClearCell(Position pos) {
    CheckPositin(pos);
    cells_.erase(pos);

    int max_row = -1;
    int max_col = -1;

    for (auto& [pos, data]  : cells_){
        max_row = std::max(max_row, pos.row);
        max_col = std::max(max_col, pos.col);
    }

    size_ = {max_row + 1, max_col + 1};
}

Size Sheet::GetPrintableSize() const {
    return size_;
}

void Sheet::PrintValues(std::ostream& output) const {
    for (int row = 0; row < size_.rows; ++row) {
        for (int col = 0; col < size_.cols; ++col) {
            auto it = cells_.find({ row, col });
            if (it != cells_.end()) {
                CellInterface::Value res = it->second->GetValue();

                if (std::holds_alternative<std::string>(res)) {
                    output << std::get<std::string>(res);
                } else if(std::holds_alternative<FormulaError>(res)) {
                    output << std::get<FormulaError>(res);
                } else {
                    output << std::get<double>(res);
                }
            }
            if (col < size_.cols - 1) {
                output << '\t';
            }
        }
        output << '\n';
    }
}
void Sheet::PrintTexts(std::ostream& output) const {
    for (int row = 0; row < size_.rows; ++row) {
        for (int col = 0; col < size_.cols; ++col) {
            auto it = cells_.find({ row, col });
            if (it != cells_.end()) {
                output << it->second->GetText();
            }
            if (col < size_.cols - 1) {
                output << '\t';
            }
        }
        output << '\n';
    }
}

bool Sheet::hasCircularReference(const Position& pos)
{
    auto sheetCell = GetCell(pos);
    if (sheetCell == nullptr)
        return false;

    std::unordered_set<Position, PositionHash> visited;
    return hasCircularReference(pos, visited);
}

bool Sheet::hasCircularReference(const Position& pos, std::unordered_set<Position, PositionHash>& visited)
{
    visited.insert(pos);

    auto sheetCell = GetCell(pos);
    if (sheetCell == nullptr)
        return false;
    const auto& cells = sheetCell->GetReferencedCells();
    for (const auto cell : cells){
        if (visited.count(cell)){
            return true;
        } else {
            return hasCircularReference(cell, visited);
        }
    }
    return false;
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}
