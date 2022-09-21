#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>


// Реализуйте следующие методы
//Cell::Cell(const SheetInterface& sheet)
//    : impl_{std::make_unique<EmptyImpl>()}
//    , sheet_{sheet}
//{ }

Cell::Cell(const SheetInterface& sheet)
    : impl_{std::make_unique<EmptyImpl>()}
    , sheet_{sheet}
{ }

Cell::~Cell() {
    if (impl_){
        impl_.reset(nullptr);
    }
}

void Cell::Set(std::string text) {
    if (text == impl_->GetText()) {
        return;
    }

    if (text.empty()){
        impl_ = std::make_unique<EmptyImpl>();
    } else if (text.size() > 1 && text.front() == FORMULA_SIGN) {
        std::unique_ptr<Impl> impl;
        impl = std::make_unique<FormulaImpl>(std::move(text), sheet_);
        impl_ = std::move(impl);
    } else {
        impl_ = std::make_unique<TextImpl>(text);
    }
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const
{
    return impl_->GetValue();
}

std::string Cell::GetText() const
{
    return impl_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const
{
    return impl_->GetReferencedCells();
}

Cell::Value Cell::EmptyImpl::GetValue() const
{
    return {};
}

std::string Cell::EmptyImpl::GetText() const
{
    return {};
}

Cell::TextImpl::TextImpl(const std::string& text)
    : text_{text}
{
}

Cell::Value Cell::TextImpl::GetValue() const
{
    if (text_.front() == ESCAPE_SIGN){
        return text_.substr(1);
    } else {
        return text_;
    }
}

std::string Cell::TextImpl::GetText() const
{
    return text_;
}

Cell::FormulaImpl::FormulaImpl(const std::string& text, const SheetInterface& sheet)
    : formula_(ParseFormula(text.substr(1)))
    , sheet_{sheet}
{

}

Cell::Value Cell::FormulaImpl::GetValue() const
{
    auto res = formula_->Evaluate(sheet_);
    if (std::holds_alternative<double>(res)) {
        return std::get<double>(res);
    } else {
        return std::get<FormulaError>(res);
    }
}

std::string Cell::FormulaImpl::GetText() const
{
    return FORMULA_SIGN + formula_->GetExpression();
}

std::vector<Position> Cell::FormulaImpl::GetReferencedCells() const
{
    return formula_->GetReferencedCells();
}

std::vector<Position> Cell::Impl::GetReferencedCells() const
{
    return {};
}
