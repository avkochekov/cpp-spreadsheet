#pragma once

#include "cell.h"
#include "common.h"
#include "formula.h"

#include <functional>
#include <unordered_set>

class Sheet;

class Cell : public CellInterface {
public:
    Cell(const SheetInterface& sheet);
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    bool IsReferenced() const;

private:
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;
    std::unique_ptr<Impl> impl_;

    const SheetInterface& sheet_;

    // needed to cache cleaning
    std::unordered_set<Position, PositionHash> pendends_;
    // needed to evaluations
    std::unordered_set<Position, PositionHash> parents_;
};

class Cell::Impl {
protected:
    Impl() = default;

public:
    virtual ~Impl() = default;

    virtual CellInterface::Value GetValue() const = 0;
    virtual std::string GetText() const = 0;

    virtual std::vector<Position> GetReferencedCells() const;
};

class Cell::EmptyImpl : public Impl{
public:
    virtual CellInterface::Value GetValue() const override;
    virtual std::string GetText() const override;
};

class Cell::TextImpl : public Impl{
public:
    TextImpl(const std::string& text);
    virtual CellInterface::Value GetValue() const override;
    virtual std::string GetText() const override;

private:
    std::string text_;
};

class Cell::FormulaImpl : public Impl{
public:
    FormulaImpl(const std::string& text, const SheetInterface& sheet);
    virtual CellInterface::Value GetValue() const override;
    virtual std::string GetText() const override;

    std::vector<Position> GetReferencedCells() const override;
private:
    std::unique_ptr<FormulaInterface> formula_;

    const SheetInterface& sheet_;
};
