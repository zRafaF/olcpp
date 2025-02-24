#pragma once

#include <memory>
#include <vector>

template <typename T>
void appendVectors(std::vector<T>& target, const std::vector<T>& source) {
    target.insert(target.end(), source.begin(), source.end());
}