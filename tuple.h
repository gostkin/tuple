#ifndef TUPLE_H
#define TUPLE_H

namespace _Tuple {
    template <typename... Args>
    class Tuple;

    template <typename T, typename... Args>
    class Tuple <T, Args...> : public Tuple <Args...> {
    private:
        T _element;

        T &element() & {
            return _element;
        }

        const T &element() const & {
            return _element;
        }

        T &&element() && {
            return std::move(_element);
        }

    public:
        Tuple() : _element{T{}} {}

        template <typename U, typename... Back, typename std::enable_if <
                !std::is_base_of <Tuple <T, Args...>, typename std::decay_t <U>>::value, int>::type = 0>
        Tuple(U &&what, Back &&...back) :
                Tuple <Args...> (std::forward <Back> (back)...),
                _element{what} {}

        Tuple(const Tuple &old) :
                Tuple <Args...> (old),
                _element{old._element} {}

        Tuple(Tuple &&old) noexcept :
                Tuple <Args...> (std::move(old)),
                _element{std::move(old._element)} {}

        ~Tuple() {}

        Tuple &operator=(const Tuple &right) {
            _element = right._element;
            Tuple <Args...>::operator=(right);
            return *this;
        }

        Tuple &operator=(Tuple &&right) noexcept {
            _element = std::move(right._element);
            Tuple <Args...>::operator=(right);
            return *this;
        }

        void swap(Tuple &right) {
            std::swap(_element, right._element);
            Tuple <Args...>::swap(right);
        }

        void assign(const Tuple <Args...> &right) {
            Tuple <Args...>::operator=(right);
        }

        template <size_t index, typename std::enable_if_t <!index, int> = 0>
        decltype (auto) i_take() & {
            return element();
        }

        template <size_t index, typename std::enable_if_t <index, int> = 0>
        decltype (auto) i_take() & {
            return static_cast <Tuple <Args...> & > (*this).i_take <index - 1> ();
        }

        template <size_t index, typename std::enable_if_t <!index, int> = 0>
        decltype (auto) i_take() const & {
            return element();
        }

        template <size_t index, typename std::enable_if_t <index, int> = 0>
        decltype (auto) i_take() const & {
            return static_cast <const Tuple <Args...> & > (*this).i_take <index - 1> ();
        }

        template <size_t index, typename std::enable_if_t <!index, int> = 0>
        decltype (auto) ir_take() && {
            return std::move(*this).element();
        }

        template <size_t index, typename std::enable_if_t <index, int> = 0>
        decltype (auto) ir_take() && {
            return static_cast <Tuple <Args...> &&> (*this).i_take <index - 1> ();
        }

        template <typename U, typename std::enable_if_t <std::is_same <U, T>::value, int> = 0>
        decltype (auto) t_take() & {
            return element();
        }

        template <typename U, typename std::enable_if_t <!std::is_same <U, T>::value, int> = 0>
        decltype (auto) t_take() & {
            return static_cast <Tuple <Args...> & > (*this).t_take <U> ();
        }

        template <typename U, typename std::enable_if_t <std::is_same <U, T>::value, int> = 0>
        decltype (auto) t_take() const & {
            return element();
        }

        template <typename U, typename std::enable_if_t <!std::is_same <U, T>::value, int> = 0>
        decltype (auto) t_take() const & {
            return static_cast <const Tuple <Args...> & > (*this).t_take <U> ();
        }

        template <typename U, typename std::enable_if_t <std::is_same <U, T>::value, int> = 0>
        decltype (auto) tr_take() && {
            return std::move(*this).element();
        }

        template <typename U, typename std::enable_if_t <!std::is_same <U, T>::value, int> = 0>
        decltype (auto) tr_take() && {
            return static_cast <Tuple <Args...> && > (*this).t_take <U> ();
        }

        bool operator<(const Tuple &right) const {
            if (_element != right._element)
                return _element < right._element;

            return static_cast <Tuple <Args...>> (*this) < static_cast <Tuple <Args...>> (right);
        }

        bool operator<=(const Tuple &right) const {
            return !(right < *this);
        }

        bool operator> (const Tuple &right) const {
            return right < *this;
        }

        bool operator>=(const Tuple &right) const {
            return !(*this < right);
        }

        bool operator==(const Tuple &right) const {
            return (right <= *this) && (right >= *this);
        }

        bool operator!=(const Tuple &right) const {
            return !(*this == right);
        }
    };

    template <>
    class Tuple <> {
    public:
        void swap(Tuple &) {};

        bool operator<(const Tuple &) const {
            return false;
        }
    };

    template <typename... Args>
    auto makeTuple(Args &&...args) {
        return Tuple <typename std::decay_t <Args>...> (std::forward <Args> (args)...);
    }

    template <size_t index, typename T, typename... Args>
    decltype (auto) get(Tuple <T, Args...> &right) {
        return right.template i_take <index> ();
    };

    template <size_t index, typename T, typename... Args>
    decltype (auto) get(const Tuple <T, Args...> &right) {
        return right.template i_take <index> ();
    };

    template <size_t index, typename T, typename... Args>
    decltype (auto) get(Tuple <T, Args...> &&right) {
        return std::move(right).template ir_take <index> ();
    };

    template <typename U, typename T, typename... Args>
    decltype (auto) get(Tuple <T, Args...> &right) {
        return right.template t_take <U> ();
    };

    template <typename U, typename T, typename... Args>
    decltype (auto) get(const Tuple <T, Args...> &right) {
        return right.template t_take <U> ();
    };

    template <typename U, typename T, typename... Args>
    decltype (auto) get(Tuple <T, Args...> &&right) {
        return std::move(right).template tr_take <U> ();
    };

    template <typename L, typename R, class... Args>
    auto tupleCat(const L &left, const R &right, const Args &...back) {
        return tupleCat(tupleCat(left, right), back...);
    };

    template <typename T, typename... L, typename... R>
    auto tupleCat(const Tuple <T, L...> &left, const Tuple <R...> &right) {
        auto temp = tupleCat(static_cast <Tuple <L...>> (left), right);

        Tuple <T, L..., R...> newTuple;
        newTuple.assign(temp);

        return newTuple;
    };

    template <typename... R>
    auto tupleCat(const Tuple <> &, const Tuple <R...> &right) {
        return right;
    }
}

using namespace _Tuple;

#endif //TUPLE_H

