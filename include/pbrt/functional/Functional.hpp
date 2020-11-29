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

    // Maps the range of integers [first, last) to the container type
    // R<G>, where G is the return type of f,
    // producing the range [f(first), ... , f(last - 1)]
    template <template <typename...> typename R,
              typename T,
              typename F,
              typename Result = R<std::invoke_result_t<F, T>>>
    Result mapIntegerRange(const T first, const T last, F f) {
        static_assert(std::is_integral_v<T> && !std::is_same_v<T, bool>,
                      "A range can only be defined by integral ends");

        Result result;

        if (const auto size = last - first; size > 0) {
            result.reserve(size);
        }

        for (auto i = first; i < last; ++i) {
            result.push_back(f(i));
        }

        return result;
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