#include "vector.hpp"

#include <iostream>
#include <vector>


#define UNIT_ASSERT_VALUES_EQUAL(A, B) {                                                           \
    while ((A) != (B)) {                                                                           \
        std::cerr << __FILE__ << ":" << __LINE__ << " Test " << __func__;                          \
        std::cerr << " failed, values "#A" = " << (A);                                             \
        std::cerr << " and "#B" = " << (B) << " is not equal" << std::endl;                        \
        std::terminate();                                                                          \
    }                                                                                              \
}

#define UNIT_ASSERT(A) UNIT_ASSERT_VALUES_EQUAL(A, true)


// Checks access to uninitialized value
class TUBChecker {
    inline static const int TEST_CONST = 1234567;  // Some random constant

    int value_ = 0;
    int init_guard_ = 0;

public:
    inline static int created_objects = 0;

    TUBChecker(int value = 0)
        : value_(value)
    {
        init();
    }

    TUBChecker(const TUBChecker& other)
        : value_(other.value_)
    {
        init();
    }

    TUBChecker(TUBChecker&& other)
        : value_(other.value_)
    {
        init();
    }

    TUBChecker& operator=(const TUBChecker& other) {
        UNIT_ASSERT_VALUES_EQUAL(init_guard_, TEST_CONST);
        value_ = other.value_;
        return *this;
    }

    TUBChecker& operator=(TUBChecker&& other) {
        UNIT_ASSERT_VALUES_EQUAL(init_guard_, TEST_CONST);
        value_ = other.value_;
        return *this;
    }

    ~TUBChecker() {
        UNIT_ASSERT_VALUES_EQUAL(init_guard_, TEST_CONST);
        UNIT_ASSERT(created_objects > 0);
        created_objects--;
    }

    bool operator==(const TUBChecker& other) {
        UNIT_ASSERT_VALUES_EQUAL(init_guard_, TEST_CONST);
        return value_ == other.value_;
    }

    bool operator==(int other) {
        UNIT_ASSERT_VALUES_EQUAL(init_guard_, TEST_CONST);
        return value_ == other;
    }

    std::strong_ordering operator<=>(const TUBChecker& other) {
        UNIT_ASSERT_VALUES_EQUAL(init_guard_, TEST_CONST);
        return value_ <=> other.value_;
    }

    int get() const {
        UNIT_ASSERT_VALUES_EQUAL(init_guard_, TEST_CONST);
        return value_;
    }

private:
    void init() {
        init_guard_ = TEST_CONST;
        created_objects++;
    }
};

std::ostream& operator<<(std::ostream& stream, const TUBChecker& checker) {
    stream << checker.get();
    return stream;
}

void test_constructors() {
    // Default constructor
    TVector<TUBChecker> vector_default;
    UNIT_ASSERT(vector_default.empty());
    UNIT_ASSERT_VALUES_EQUAL(vector_default.size(), 0);
    UNIT_ASSERT_VALUES_EQUAL(vector_default.max_size(), std::numeric_limits<ptrdiff_t>::max() / sizeof(TUBChecker));
    UNIT_ASSERT_VALUES_EQUAL(vector_default.capacity(), 0);
    UNIT_ASSERT_VALUES_EQUAL(vector_default.data(), nullptr);

    // Size constructor
    TVector<TVector<TUBChecker>> vector_size(2);
    UNIT_ASSERT(!vector_size.empty());
    UNIT_ASSERT_VALUES_EQUAL(vector_size.size(), 2);
    UNIT_ASSERT_VALUES_EQUAL(vector_size.capacity(), 2);
    UNIT_ASSERT(vector_size[0].empty());

    // Size initialized constructor
    TVector<TVector<TUBChecker>> vector_size_initialized(3, TVector<TUBChecker>(3));
    UNIT_ASSERT_VALUES_EQUAL(vector_size_initialized.front().size(), 3);
    UNIT_ASSERT_VALUES_EQUAL(vector_size_initialized.at(1).size(), 3);
    UNIT_ASSERT_VALUES_EQUAL(vector_size_initialized.back().size(), 3);

    // Initializer list constructor
    TVector<TUBChecker> vector_initialized({0, 1, 2});
    UNIT_ASSERT_VALUES_EQUAL(vector_initialized.front(), 0);
    UNIT_ASSERT_VALUES_EQUAL(vector_initialized.at(1), 1);
    UNIT_ASSERT_VALUES_EQUAL(vector_initialized.back(), 2);
    UNIT_ASSERT_VALUES_EQUAL(vector_initialized.size(), 3);
    UNIT_ASSERT_VALUES_EQUAL(vector_initialized.capacity(), 3);

    // Iterators constructor
    std::vector<TUBChecker> sample_vector = { 1, 2, 3 };
    TVector<TUBChecker> from_sample_vector(sample_vector.begin(), sample_vector.end());
    UNIT_ASSERT_VALUES_EQUAL(from_sample_vector.size(), 3);
    UNIT_ASSERT_VALUES_EQUAL(from_sample_vector[0], 1);
    UNIT_ASSERT_VALUES_EQUAL(from_sample_vector[1], 2);
    UNIT_ASSERT_VALUES_EQUAL(from_sample_vector[2], 3);

    // Copy constructor
    TVector<TVector<TUBChecker>> vector_copy(vector_size_initialized);
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
    TVector<TVector<TUBChecker>> vector(2, {1, 2});
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
    TVector<TUBChecker> vector;

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
    TVector<TVector<TUBChecker>> nested_vector;
    UNIT_ASSERT_VALUES_EQUAL(nested_vector.emplace_back(1, -1).size(), 1);
    UNIT_ASSERT_VALUES_EQUAL(nested_vector.size(), 1);
    UNIT_ASSERT_VALUES_EQUAL(nested_vector[0][0], -1);
}

void test_compare() {
    // Equal compare
    TVector<TUBChecker> left = { 1, 2, 3 };
    TVector<TUBChecker> right = { 1, 2, 3 };
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
    static_assert(std::contiguous_iterator<TVector<TUBChecker>::iterator>);

    // Direct iterator
    TVector<TUBChecker> vector = { 1, 2, 3, 4, 5 };
    vector.reserve(vector.size() * 2);

    size_t i = 1;
    for (auto element : vector) {
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
    for (auto element : const_vector) {
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

void test_advanced_modifiers() {
    // Test erase
    TVector<TUBChecker> vector_erase = { 1, 2, 3, 4, 5, 6 };
    auto erased_value = *vector_erase.erase(vector_erase.begin() + 1, vector_erase.begin() + 3);
    UNIT_ASSERT_VALUES_EQUAL(erased_value, 4);
    UNIT_ASSERT(vector_erase == TVector<TUBChecker>({1, 4, 5, 6}));

    auto erased_it = vector_erase.erase(vector_erase.begin() + 3);
    UNIT_ASSERT(erased_it == vector_erase.end());
    UNIT_ASSERT(vector_erase == TVector<TUBChecker>({1, 4, 5}));

    // Test emplace
    TVector<TVector<TUBChecker>> vector_emplace;
    vector_emplace.emplace(vector_emplace.begin(), 1, -1);
    vector_emplace.emplace(vector_emplace.begin(), 2, 3);
    UNIT_ASSERT(vector_emplace == TVector<TVector<TUBChecker>>({{3, 3}, {-1}}));

    auto emplace_it = vector_emplace.emplace(vector_emplace.begin() + 1, 3, 5);
    UNIT_ASSERT(emplace_it == vector_emplace.begin() + 1);
    UNIT_ASSERT(vector_emplace == TVector<TVector<TUBChecker>>({{3, 3}, {5, 5, 5}, {-1}}));

    // Test insert
    TVector<TUBChecker> vector_insert;

    vector_insert.insert(vector_insert.begin(), 1);
    UNIT_ASSERT(vector_insert == TVector<TUBChecker>({1}));

    vector_insert.insert(vector_insert.begin(), 2, -1);
    UNIT_ASSERT(vector_insert == TVector<TUBChecker>({-1, -1, 1}));

    auto insert_it = vector_insert.insert(vector_insert.begin() + 1, {5, 6});
    UNIT_ASSERT(insert_it == vector_insert.begin() + 1);
    UNIT_ASSERT(vector_insert == TVector<TUBChecker>({-1, 5, 6, -1, 1}));

    vector_insert.insert(vector_insert.begin() + 3, vector_erase.begin(), vector_erase.end());
    UNIT_ASSERT(vector_insert == TVector<TUBChecker>({-1, 5, 6, 1, 4, 5, -1, 1}));
}

int main() {
    test_constructors();
    test_capacity();
    test_modifiers();
    test_compare();
    test_iterators();
    test_advanced_modifiers();

    // Check destructors calling
    UNIT_ASSERT_VALUES_EQUAL(TUBChecker::created_objects, 0);

    std::cout << "Tests succesfully passed!\n";

    return 0;
}
