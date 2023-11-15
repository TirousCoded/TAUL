

#pragma once


#include <cstddef>
#include <array>

#include "asserts.h"


namespace taul {


	// TODO: maybe make the counters in the below atomic if we ever need thread-safety


	// Error is expected to be an enum w/ an element 'num' specifying
	// the number of elements in the enum, which is not itself a
	// valid error

	template<typename Error>
	class error_counter final {
	public:

		using error = typename Error;

		static constexpr std::size_t errors = (std::size_t)error::num;


		error_counter() = default;
		error_counter(const error_counter<error>&) = delete;
		inline error_counter(error_counter<error>&& x) noexcept;

		~error_counter() noexcept = default;

		error_counter<error>& operator=(const error_counter<error>&) = delete;
		inline error_counter<error>& operator=(error_counter<error>&& rhs) noexcept;


		// behaviour is undefined if x == error::num

		inline std::size_t count(error x) const noexcept;

		inline std::size_t total() const noexcept;


		// behaviour is undefined if x == error::num

		inline void raise(error x) noexcept;


		inline void reset() noexcept;


	private:

		std::array<std::size_t, errors> _counts = {};
		std::size_t _total = 0;
	};

	template<typename Error>
	inline taul::error_counter<Error>::error_counter(error_counter<error>&& x) noexcept 
		: _counts(std::move(x._counts)), 
		_total(std::move(x._total)) {}

	template<typename Error>
	inline error_counter<Error>& taul::error_counter<Error>::operator=(error_counter<error>&& rhs) noexcept {
		if (&rhs != this) {
			_counts = std::move(rhs._counts);
			_total = std::move(rhs._total);
		}
		return *this;
	}

	template<typename Error>
	inline std::size_t taul::error_counter<Error>::count(error x) const noexcept {
		TAUL_ASSERT(x != error::num);
		return _counts[(std::size_t)x];
	}

	template<typename Error>
	inline std::size_t taul::error_counter<Error>::total() const noexcept {
		return _total;
	}

	template<typename Error>
	inline void taul::error_counter<Error>::raise(error x) noexcept {
		TAUL_ASSERT(x != error::num);
		_counts[(std::size_t)x]++;
		_total++;
	}

	template<typename Error>
	inline void taul::error_counter<Error>::reset() noexcept {
		_counts.fill(0);
		_total = 0;
	}
}

