#include <algorithm>


template <typename TItem>
class TVector {
public:
    using value_type = TItem;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = TItem&;
    using const_reference = const TItem&;

public:
    TVector() = default;

    explicit TVector(size_type size, const value_type& value = value_type())
        : size_(size)
    {
        reserve(size_);
        for (size_type i = 0; i < size_; ++i) {
            new (begin_ + i) value_type(value);
        }
    }

    TVector(std::initializer_list<value_type> init)
        : size_(init.size())
    {
        reserve(size_);
        for (size_type i = 0; const value_type& value : init) {
            new (begin_ + i++) value_type(value);
        }
    }

    TVector(const TVector& other)
        : size_(other.size())
    {
        reserve(size_);
        for (size_type i = 0; i < size_; ++i) {
            new (begin_ + i) value_type(other[i]);
        }
    }

    TVector(TVector&& other) noexcept {
        swap(other);
    }

    ~TVector() {
        if (begin_ != nullptr) {
            free(begin_);
            begin_ = nullptr;
            end_ = nullptr;
        }
    }

    TVector& operator=(const TVector& other) {
        copy_and_swap(other);
        return *this;
    }

    TVector& operator=(TVector&& other) noexcept {
        swap(other);
        return *this;
    }

    TVector& operator=(std::initializer_list<value_type> init) {
        copy_and_swap(std::move(init));
        return *this;
    }

    void assign(size_type count, const value_type& value) {
        copy_and_swap(count, value);
    }

    void assign(std::initializer_list<value_type> init) {
        copy_and_swap(std::move(init));
    }

public:
    reference at(size_type position) {
        return begin_[position];
    }

    const_reference at(size_type position) const {
        return begin_[position];
    }

    reference operator[](size_type position) {
        return begin_[position];
    }

    const_reference operator[](size_type position) const {
        return begin_[position];
    }

    reference front() {
        return at(0);
    }

    const_reference front() const {
        return at(0);
    }

    reference back() {
        return at(size_ - 1);
    }

    const_reference back() const {
        return at(size_ - 1);
    }

    value_type* data() {
        return begin_;
    }

    const value_type* data() const {
        return begin_;
    }

public:
    bool empty() const {
        return size_ == 0;
    }

    size_type size() const {
        return size_;
    }

    size_type max_size() const {
        return std::numeric_limits<difference_type>::max() / sizeof(value_type);
    }

    void reserve(size_type new_capacity) {
        if (new_capacity > capacity()) {
            reallocate(new_capacity);
        }
    }

    size_type capacity() const {
        return std::distance(begin_, end_);
    }

    void shrink_to_fit() {
        if (size_ == 0 && capacity() > 0) {
            free(begin_);
            begin_ = nullptr;
            end_ = nullptr;
        } else if (size_ < capacity()) {
            reallocate(size_);
        }
    }

public:
    void clear() {
        for (size_type i = 0; i < size_; ++i) {
            begin_[i].~TItem();
        }
        size_ = 0;
    }

    void push_back(value_type value) {
        if (size_ == capacity()) {
            reserve(std::max(2 * size_, static_cast<size_type>(1)));
        }

        new (begin_ + size_++) value_type(std::move(value));
    }

    template <typename... TArgs>
    reference emplace_back(TArgs&&... args) {
        if (size_ == capacity()) {
            reserve(std::max(2 * size_, static_cast<size_type>(1)));
        }

        return *(new (begin_ + size_++) value_type(std::forward<TArgs>(args)...));
    }

    void pop_back() {
        begin_[size_--].~TItem();
    }

    void resize(size_type count) {
        resize(count, value_type());
    }

    void resize(size_type count, const value_type& value) {
        reserve(count);
        for (size_type i = count; i < size_; ++i) {
            begin_[i].~TItem();
        }
        for (size_type i = size_; i < count; ++i) {
            new (begin_ + i) value_type(value);
        }
        size_ = count;
    }

    void swap(TVector& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(begin_, other.begin_);
        std::swap(end_, other.end_);
    }

public:
    bool operator==(const TVector& other) const {
        if (size_ != other.size_) {
            return false;
        }

        for (size_type i = 0; i < size_; ++i) {
            if (begin_[i] != other[i]) {
                return false;
            }
        }

        return true;
    }

    std::strong_ordering operator<=>(const TVector& other) const {
        if (*this == other) {
            return std::strong_ordering::equal;
        }

        for (size_type i = 0; i < std::min(size_, other.size_); ++i) {
            if (begin_[i] != other[i]) {
                return begin_[i] <=> other[i];
            }
        }

        return size_ <=> other.size_;
    }

private:
    template <typename... TArgs>
    void copy_and_swap(TArgs&&... args) {
        TVector object_copy(std::forward<TArgs>(args)...);
        swap(object_copy);
    }

    void reallocate(size_t new_capacity) {
        if (value_type* new_begin = reinterpret_cast<value_type*>(realloc(begin_, new_capacity * sizeof(value_type)))) {
            begin_ = new_begin;
            end_ = begin_ + new_capacity;
        } else {
            std::abort();
        }
    }

private:
    size_type size_ = 0;
    value_type* begin_ = nullptr;
    value_type* end_ = nullptr;
};
