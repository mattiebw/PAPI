#pragma once

#define BIT(x) (1 << (x))

// MSVC compiler intrinsics
// MW @todo: Add intrinsics for other compilers (gcc/clang)
// MW @todo: Consider adding FORCEINLINE_DEBUGGABLE (does not inline in debug builds)
#ifdef PAPI_PLATFORM_WINDOWS
	#define UNALIGNED __unaligned
	#define FORCEINLINE __forceinline
#else
	#define UNALIGNED
	#define FORCEINLINE inline
#endif

#if __cplusplus >= 201703L or (defined(_MSVC_LANG) and _MSVC_LANG >= 201703L)
	#define NODISCARD [[nodiscard]]
#else
	#define NODISCARD
#endif

// -----------------------------------------------------------------------------------------------
// Array size macro
// From winnt.h:
//
// RtlpNumberOf is a function that takes a reference to an array of N Ts.
//
// typedef T array_of_T[N];
// typedef array_of_T &reference_to_array_of_T;
//
// RtlpNumberOf returns a pointer to an array of N chars.
// We could return a reference instead of a pointer but older compilers do not accept that.
//
// typedef char array_of_char[N];
// typedef array_of_char *pointer_to_array_of_char;
//
// sizeof(array_of_char) == N
// sizeof(*pointer_to_array_of_char) == N
//
// pointer_to_array_of_char RtlpNumberOf(reference_to_array_of_T);
//
// We never even call RtlpNumberOf, we just take the size of dereferencing its return type.
// We do not even implement RtlpNumberOf, we just declare it.
//
// Attempts to pass pointers instead of arrays to this macro result in compile time errors.
// That is the point.
//
extern "C++" // templates cannot be declared to have 'C' linkage
template <typename T, size_t N>
char (* RtlpNumberOf(UNALIGNED T (&)[N]))[N];
#define PAPI_ARRAY_SIZE(A) (sizeof(*RtlpNumberOf(A)))
// -----------------------------------------------------------------------------------------------

// Basic swap macro
#define PAPI_SWAP(IntermediateType, a, b) do { IntermediateType t = a; (a) = b; (b) = t; } while (0)

template <typename T>
using Scope = std::unique_ptr<T>;

template <typename T, typename... Args>
constexpr Scope<T> CreateScope(Args&&... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename... Args>
constexpr Ref<T> CreateRef(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

#define PAPI_ASSERT SDL_assert

struct SemVer
{
	short Major;
	short Minor;
	short Patch;

	SemVer() : Major(0), Minor(0), Patch(0) {}
	explicit SemVer(const short major) : Major(major), Minor(0), Patch(0) {}
	SemVer(const short major, const short minor, const short patch) : Major(major), Minor(minor), Patch(patch) {}

	// SemVer comparison operators
	bool operator==(const SemVer& other) const
	{
		return Major == other.Major && Minor == other.Minor && Patch == other.Patch;
	}

	bool operator!=(const SemVer& other) const
	{
		return !(*this == other);
	}

	bool operator<(const SemVer& other) const
	{
		if (Major < other.Major)
			return true;
		if (Major > other.Major)
			return false;

		if (Minor < other.Minor)
			return true;
		if (Minor > other.Minor)
			return false;

		return Patch < other.Patch;
	}
};
