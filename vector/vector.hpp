#include <algorithm>
#include <iterator>
#include <limits>


template <typename TItem, typename TItemPointer = TItem*, typename TItemReference = TItem&>
class TVectorIterator {
public:
    using difference_type = std::ptrdiff_t;

public:
    TVectorIterator() = default;

    explicit TVectorIterator(TItemPointer pointer)
        : pointer_(pointer)
    {}

    TVectorIterator(const TVectorIterator<TItem>& iterator)
        : pointer_(iterator.get())
    {}

public:
    TItemReference operator*() const {
        return *pointer_;
    }

    TItemPointer operator->() const {
        return pointer_;
    }

    TItemReference operator[](difference_type shift) const {
        return pointer_[shift];
    }

    TVectorIterator& operator=(TVectorIterator other) {
        pointer_ = other.pointer_;
        return *this;
    }

    TItemPointer get() const {
        return pointer_;
    }

public:
    TVectorIterator& operator++() {
        ++pointer_;
        return *this;
    }

    TVectorIterator operator++(int) {
        TVectorIterator result(*this);
        ++pointer_;
        return result;
    }

    TVectorIterator& operator--() {
        --pointer_;
        return *this;
    }

    TVectorIterator operator--(int) {
        TVectorIterator result(*this);
        --pointer_;
        return result;
    }

public:
    TVectorIterator operator+(difference_type shift) const {
        TVectorIterator result(*this);
        result += shift;
        return result;
    }

    TVectorIterator operator-(difference_type shift) const {
        TVectorIterator result(*this);
        result -= shift;
        return result;
    }

    difference_type operator-(TVectorIterator other) const {
        return pointer_ - other.pointer_;
    }

    TVectorIterator& operator+=(difference_type shift) {
        pointer_ += shift;
        return *this;
    }

    TVectorIterator& operator-=(difference_type shift) {
        pointer_ -= shift;
        return *this;
    }

public:
    bool operator==(const TVectorIterator& other) const {
        return pointer_ == other.pointer_;
    }

    std::strong_ordering operator<=>(const TVectorIterator& other) const {
        return pointer_ <=> other.pointer_;
    }

private:
    TItemPointer pointer_ = nullptr;
};

template <typename TItem, typename TItemPointer, typename TItemReference>
TVectorIterator<TItem, TItemPointer, TItemReference> operator+(typename TVectorIterator<TItem, TItemPointer, TItemReference>::difference_type shift, TVectorIterator<TItem, TItemPointer, TItemReference> it) {
    return it + shift;
}

template <typename TItem, typename TItemPointer, typename TItemReference>
struct std::iterator_traits<TVectorIterator<TItem, TItemPointer, TItemReference>> {
    using difference_type = typename TVectorIterator<TItem, TItemPointer, TItemReference>::difference_type;
    using value_type = TItem;
    using pointer = TItemPointer;
    using reference = TItemReference;
    using iterator_category = std::contiguous_iterator_tag;
};

template <typename TItem>
class TVector {
public:
    using value_type = TItem;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = TItem&;
    using const_reference = const TItem&;
    using pointer = TItem*;
    using const_pointer = const TItem*;
    using iterator = TVectorIterator<value_type, pointer, reference>;
    using const_iterator = TVectorIterator<value_type, const_pointer, const_reference>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

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

    template <std::input_iterator TInputIterator>
    TVector(TInputIterator first, TInputIterator last)
        : size_(std::distance(first, last))
    {
        reserve(size_);
        for (size_type i = 0; first != last; ++i, ++first) {
            new (begin_ + i) value_type(*first);
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
        resize(0);
        shrink_to_fit();
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

    template <std::input_iterator TInputIterator>
    void assign(TInputIterator first, TInputIterator last) {
        copy_and_swap(first, last);
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

    pointer data() {
        return begin_;
    }

    const_pointer data() const {
        return begin_;
    }

public:
    iterator begin() {
        return iterator(begin_);
    }

    const_iterator begin() const {
        return const_iterator(begin_);
    }

    iterator end() {
        return begin() + size_;
    }

    const_iterator end() const {
        return begin() + size_;
    }

    reverse_iterator rbegin() {
        return std::make_reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const {
        return std::make_reverse_iterator(end());
    }

    reverse_iterator rend() {
        return std::make_reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return std::make_reverse_iterator(begin());
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

    iterator insert(const_iterator it, const value_type& value) {
        return emplace(it, value);
    }

    iterator insert(const_iterator it, value_type&& value) {
        return emplace(it, std::move(value));
    }

    iterator insert(const_iterator it, size_type count, const value_type& value) {
        difference_type pos = it - begin();
        move_range(pos, count);
        for (size_type i = pos; i < pos + count; ++i) {
            begin_[i] = value;
        }
        return begin() + pos;
    }

    iterator insert(const_iterator it, std::initializer_list<value_type> init) {
        difference_type pos = it - begin();
        move_range(pos, init.size());
        for (size_type i = pos; const value_type& value : init) {
            begin_[i++] = value;
        }
        return begin() + pos;
    }

    template <std::input_iterator TInputIterator>
    iterator insert(const_iterator it, TInputIterator first, TInputIterator last) {
        difference_type pos = it - begin();
        move_range(pos, std::distance(first, last));
        for (size_type i = pos; first != last; ++i, ++first) {
            begin_[i] = *first;
        }
        return begin() + pos;
    }

    template <typename... TArgs>
    iterator emplace(const_iterator it, TArgs&&... args) {
        difference_type pos = it - begin();
        move_range(pos, 1);
        begin_[pos] = value_type(std::forward<TArgs>(args)...);
        return begin() + pos;
    }

    iterator erase(const_iterator it) {
        difference_type pos = it - begin();
        erase_range(pos, 1);
        return begin() + pos;
    }

    iterator erase(const_iterator first, const_iterator last) {
        difference_type pos = first - begin();
        erase_range(pos, last - first);
        return begin() + pos;
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
        begin_[--size_].~TItem();
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

    void move_range(size_type pos, size_type count) {
        if (count == 0) {
            return;
        }

        resize(size_ + count);
        for (size_type i = size_ - 1; i >= pos + count; --i) {
            std::swap(begin_[i], begin_[i - count]);
        }
    }

    void erase_range(size_type pos, size_type count) {
        if (count == 0) {
            return;
        }

        for (size_type i = pos; i + count < size_; ++i) {
            std::swap(begin_[i], begin_[i + count]);
        }
        resize(size_ - count);
    }

    void reallocate(size_t new_capacity) {
        if (pointer new_begin = reinterpret_cast<pointer>(realloc(begin_, new_capacity * sizeof(value_type)))) {
            begin_ = new_begin;
            end_ = begin_ + new_capacity;
        } else {
            std::abort();
        }
    }

private:
    size_type size_ = 0;
    pointer begin_ = nullptr;
    pointer end_ = nullptr;
};
