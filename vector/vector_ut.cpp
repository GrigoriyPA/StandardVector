#include "vector.hpp"

#include <iostream>
#include <vector>


#define UNIT_ASSERT_VALUES_EQUAL(A, B) {                                                           \
    while ((A) != (B)) {                                                                           \
        std::cerr << __FILE__ << ":" << __LINE__ << " Test " << __func__;                          \
        std::cerr << " failed, values "#A" = " << (A);                                             \
        std::cerr << " and "#B" = " << (B) << " is not equal\n";                                   \
        exit(1);                                                                                   \
    }                                                                                              \
}

#define UNIT_ASSERT(A) UNIT_ASSERT_VALUES_EQUAL(A, true)


void test_constructors() {
    // Default constructor
    TVector<int> vector_default;
    UNIT_ASSERT(vector_default.empty(), true);
    UNIT_ASSERT_VALUES_EQUAL(vector_default.size(), 0);
    UNIT_ASSERT_VALUES_EQUAL(vector_default.max_size(), std::numeric_limits<ptrdiff_t>::max() / sizeof(int));
    UNIT_ASSERT_VALUES_EQUAL(vector_default.capacity(), 0);
    UNIT_ASSERT_VALUES_EQUAL(vector_default.data(), nullptr);

    // Size constructor
    TVector<TVector<int>> vector_size(2);
    UNIT_ASSERT(!vector_size.empty());
    UNIT_ASSERT_VALUES_EQUAL(vector_size.size(), 2);
    UNIT_ASSERT_VALUES_EQUAL(vector_size.capacity(), 2);
    UNIT_ASSERT(vector_size[0].empty(), true);

    // Size initialized constructor
    TVector<TVector<int>> vector_size_initialized(3, TVector<int>(3));
    UNIT_ASSERT_VALUES_EQUAL(vector_size_initialized.front().size(), 3);
    UNIT_ASSERT_VALUES_EQUAL(vector_size_initialized.at(1).size(), 3);
    UNIT_ASSERT_VALUES_EQUAL(vector_size_initialized.back().size(), 3);

    // Initializer list constructor
    TVector<int> vector_initialized({0, 1, 2});
    UNIT_ASSERT_VALUES_EQUAL(vector_initialized.front(), 0);
    UNIT_ASSERT_VALUES_EQUAL(vector_initialized.at(1), 1);
    UNIT_ASSERT_VALUES_EQUAL(vector_initialized.back(), 2);
    UNIT_ASSERT_VALUES_EQUAL(vector_initialized.size(), 3);
    UNIT_ASSERT_VALUES_EQUAL(vector_initialized.capacity(), 3);

    // Iterators constructor
    std::vector<int> sample_vector = { 1, 2, 3 };
    TVector<int> from_sample_vector(sample_vector.begin(), sample_vector.end());
    UNIT_ASSERT_VALUES_EQUAL(from_sample_vector.size(), 3);
    UNIT_ASSERT_VALUES_EQUAL(from_sample_vector[0], 1);
    UNIT_ASSERT_VALUES_EQUAL(from_sample_vector[1], 2);
    UNIT_ASSERT_VALUES_EQUAL(from_sample_vector[2], 3);

    // Copy constructor
    TVector<TVector<int>> vector_copy(vector_size_initialized);
    UNIT_ASSERT_VALUES_EQUAL(vector_copy.size(), 3);
    UNIT_ASSERT_VALUES_EQUAL(vector_copy.front().size(), 3);

    // Assign operator
    vector_default = vector_initialized;
    UNIT_ASSERT_VALUES_EQUAL(vector_default[1], 1);

    // Sized assign
    vector_default.assign(2, 3);
    UNIT_ASSERT_VALUES_EQUAL(vector_default.size(), 2);
    UNIT_ASSERT_VALUES_EQUAL(vector_default.front(), 3);
    UNIT_ASSERT_VALUES_EQUAL(vector_default.back(), 3);

    // Initializer list assign
    vector_default.assign({0, 5, 1});
    UNIT_ASSERT_VALUES_EQUAL(vector_default.size(), 3);
    UNIT_ASSERT_VALUES_EQUAL(vector_default.front(), 0);
    UNIT_ASSERT_VALUES_EQUAL(vector_default[1], 5);
    UNIT_ASSERT_VALUES_EQUAL(vector_default.back(), 1);

    // Iterators assign
    vector_default.assign(sample_vector.begin(), sample_vector.end());
    UNIT_ASSERT_VALUES_EQUAL(from_sample_vector.size(), 3);
    UNIT_ASSERT_VALUES_EQUAL(from_sample_vector[0], 1);
    UNIT_ASSERT_VALUES_EQUAL(from_sample_vector[1], 2);
    UNIT_ASSERT_VALUES_EQUAL(from_sample_vector[2], 3);
}

void test_capacity() {
    TVector<TVector<int>> vector(2, {1, 2});
    UNIT_ASSERT_VALUES_EQUAL(vector[0][0], 1);
    UNIT_ASSERT_VALUES_EQUAL(vector[0][1], 2);
    UNIT_ASSERT_VALUES_EQUAL(vector[1][0], 1);
    UNIT_ASSERT_VALUES_EQUAL(vector[1][1], 2);

    // Element assigment
    vector[0][0] = -1;
    vector[1] = { 4, 5 };
    UNIT_ASSERT_VALUES_EQUAL(vector[0][0], -1);
    UNIT_ASSERT_VALUES_EQUAL(vector[0][1], 2);
    UNIT_ASSERT_VALUES_EQUAL(vector[1][0], 4);
    UNIT_ASSERT_VALUES_EQUAL(vector[1][1], 5);

    // Reserve to bigger size
    vector.reserve(4);
    UNIT_ASSERT_VALUES_EQUAL(vector.size(), 2);
    UNIT_ASSERT_VALUES_EQUAL(vector.capacity(), 4);

    // Resize to bigger size
    vector.resize(3, { 1, 2 });
    UNIT_ASSERT_VALUES_EQUAL(vector.size(), 3);
    UNIT_ASSERT_VALUES_EQUAL(vector.capacity(), 4);
    UNIT_ASSERT_VALUES_EQUAL(vector[2][0], 1);
    UNIT_ASSERT_VALUES_EQUAL(vector[2][1], 2);

    // Resize to short size
    vector.resize(1, { -1 });
    UNIT_ASSERT_VALUES_EQUAL(vector.size(), 1);
    UNIT_ASSERT_VALUES_EQUAL(vector.capacity(), 4);

    // Test shrink to fit
    vector.shrink_to_fit();
    UNIT_ASSERT_VALUES_EQUAL(vector.size(), 1);
    UNIT_ASSERT_VALUES_EQUAL(vector.capacity(), 1);
}

void test_modifiers() {
    TVector<int> vector;

    // Test push back
    size_t size = 5;
    for (int i = 0; i < size; ++i) {
        vector.push_back(i);
        UNIT_ASSERT_VALUES_EQUAL(vector.size(), i + 1);
        UNIT_ASSERT_VALUES_EQUAL(vector.back(), i);
        UNIT_ASSERT_VALUES_EQUAL(vector[i], i);
    }
    UNIT_ASSERT_VALUES_EQUAL(vector.front(), 0);

    // Test pop back
    vector.pop_back();
    UNIT_ASSERT_VALUES_EQUAL(vector.size(), size - 1);

    // Test clear
    vector.clear();
    UNIT_ASSERT_VALUES_EQUAL(vector.size(), 0);

    // Test emplace back
    TVector<TVector<int>> nested_vector;
    UNIT_ASSERT_VALUES_EQUAL(nested_vector.emplace_back(1, -1).size(), 1);
    UNIT_ASSERT_VALUES_EQUAL(nested_vector.size(), 1);
    UNIT_ASSERT_VALUES_EQUAL(nested_vector[0][0], -1);
}

void test_compare() {
    // Equal compare
    TVector<int> left = { 1, 2, 3 };
    TVector<int> right = { 1, 2, 3 };
    UNIT_ASSERT(left == right);

    // Greater and less compare
    right = { 1, 1, 3 };
    UNIT_ASSERT(left > right);

    right = { 2, 1, 3 };
    UNIT_ASSERT(left < right);

    right = { 1, 2 };
    UNIT_ASSERT(left > right);
}

void test_iterators() {
    // Iterator is contiguous
    static_assert(std::contiguous_iterator<TVector<int>::iterator>);

    // Direct iterator
    TVector<int> vector = { 1, 2, 3, 4, 5 };
    size_t i = 1;
    for (int element : vector) {
        UNIT_ASSERT_VALUES_EQUAL(element, i);
        i++;
    }
    UNIT_ASSERT_VALUES_EQUAL(i, 6);

    // Reverse iterator
    i = 0;
    for (auto it = vector.rbegin(); it != vector.rend(); ++it) {
        UNIT_ASSERT_VALUES_EQUAL(*it, vector.size() - i);
        i++;
    }
    UNIT_ASSERT_VALUES_EQUAL(i, 5);

    // Direct const iterator
    const auto& const_vector = vector;
    i = 1;
    for (int element : const_vector) {
        UNIT_ASSERT_VALUES_EQUAL(element, i);
        i++;
    }
    UNIT_ASSERT_VALUES_EQUAL(i, 6);

    // Reverse const iterator
    i = 0;
    for (auto it = vector.rbegin(); it != vector.rend(); ++it) {
        UNIT_ASSERT_VALUES_EQUAL(*it, vector.size() - i);
        i++;
    }
    UNIT_ASSERT_VALUES_EQUAL(i, 5);
}


int main() {
    test_constructors();
    test_capacity();
    test_modifiers();
    test_compare();
    test_iterators();

    return 0;
}
