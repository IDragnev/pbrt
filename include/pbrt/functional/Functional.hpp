#pragma once

#include <type_traits>
#include <utility>

namespace idragnev::pbrt::functional {
    namespace detail {
        // Maps the elements of the given container with f,
        // returning a container of type C<G>, where G is the return type of f.
        template <bool Enumerate,
                  template <typename...> typename C,
                  typename T,
                  typename F>
        auto fmapImpl(const C<T>& c, F f) {
            using G =
                std::conditional_t<Enumerate,
                                   std::invoke_result<F, const T&, std::size_t>,
                                   std::invoke_result<F, const T&>>;
            using R = C<typename G::type>;

            R result;
            result.reserve(c.size());

            if constexpr (Enumerate) {
                std::size_t i = 0;
                for (const auto& e : c) {
                    result.push_back(f(e, i));
                    ++i;
                }
            }
            else {
                for (const auto& e : c) {
                    result.push_back(f(e));
                }
            }

            return result;
        }

        // Maps the elements of the given container with f,
        // calling std::move on each element and then calling f with it.
        // Returns a container of type C<G>, where G is the return type of f.
        template <bool Enumerate,
                  template <typename...> typename C,
                  typename T,
                  typename F>
        auto fmapImpl(C<T>&& c, F f) {
            using G =
                std::conditional_t<Enumerate,
                                   std::invoke_result<F, T&&, std::size_t>,
                                   std::invoke_result<F, T&&>>;
            using R = C<typename G::type>;

            R result;
            result.reserve(c.size());

            if constexpr (Enumerate) {
                std::size_t i = 0;
                for (auto& e : c) {
                    result.push_back(f(std::move(e), i));
                    ++i;
                }
            }
            else {
                for (auto& e : c) {
                    result.push_back(f(std::move(e)));
                }
            }

            return result;
        }
    } // namespace detail

    template <typename C, typename F>
    inline auto fmap(C&& c, F f) {
        return detail::fmapImpl<false>(std::forward<C>(c), f);
    }

    template <typename C, typename F>
    inline auto fmapIndexed(C&& c, F f) {
        return detail::fmapImpl<true>(std::forward<C>(c), f);
    }

    // exclusive range: [first, last)
    // first <= last always holds
    template <typename T>
    class IntegerRange
    {
        static_assert(std::is_integral_v<T> && !std::is_same_v<T, bool>,
                      "T must be an integral type");

    public:
        IntegerRange() = default;
        constexpr IntegerRange(const T f, const T l) noexcept
            : _first(f)
            , _last(l) {
            if (f > l) {
                *this = {};
            }
        }

        constexpr T first() const noexcept { return _first; }
        constexpr T last() const noexcept { return _last; }

        constexpr std::size_t size() const noexcept {
            return static_cast<std::size_t>(_last - _first);
        }
        constexpr bool isEmpty() const noexcept { return size() == 0u; }

    private:
        T _first = 0;
        T _last = 0;
    };

    template <typename T>
    IntegerRange(T, T) -> IntegerRange<T>;

    // Maps the range [first, last) to the container type
    // R<G>, where G is the return type of f,
    // producing the range [f(first), ... , f(last - 1)]
    template <template <typename...> typename R,
              typename T,
              typename F,
              typename Result = R<std::invoke_result_t<F, T>>>
    Result fmap(const IntegerRange<T> range, F f) {
        Result result;

        if (const auto size = range.size(); size > 0) {
            result.reserve(size);
        }

        for (auto i = range.first(); i < range.last(); ++i) {
            result.push_back(f(i));
        }

        return result;
    }

    template <typename T, typename R, typename F>
    R foldl(const IntegerRange<T> range, R acc, F f) {
        static_assert(std::is_invocable_r_v<R, F, R, T>,
                      "f must have the signature R(R, T)");
        for (auto i = range.first(); i < range.last(); ++i) {
            acc = f(std::move(acc), i);
        }

        return acc;
    }

    template <typename Callable>
    class ScopedFn
    {
    private:
        static_assert(std::is_nothrow_invocable_v<Callable>);
        static_assert(std::is_nothrow_move_constructible_v<Callable>);

    public:
        ScopedFn(Callable f) noexcept : f{std::move(f)} {}
        ~ScopedFn() { f(); }

        ScopedFn(const ScopedFn&) = delete;
        ScopedFn& operator=(const ScopedFn&) = delete;

    private:
        Callable f;
    };
} // namespace idragnev::pbrt::functional