#pragma once
#include <memory>
#include <sstream>
#include <variant>
#include <optional>
#include <functional>
#include <fstream>
#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

namespace sdk {
	namespace utilities {

        //  C++ wrapper helper functions
#ifdef _WIN32

		//----------------------------------------------------------------------------
		//	MFC style unicode conversion functions
		static inline CStringA utf16to8(const CStringW& utf16)
		{
			// Special case of empty input string
			if (utf16.IsEmpty())
			{
				// Return empty string
				return CStringA();
			}

			// Get the length, in chars, of the resulting UTF-8 string
			const int utf8Length = ::WideCharToMultiByte(
				CP_UTF8,       // convert to UTF-8
				WC_ERR_INVALID_CHARS,              // conversion flags
				utf16.GetString(),  // source UTF-16 string
				utf16.GetLength(),  // length of source UTF-16 string, in WCHARs
				nullptr,            // unused - no conversion required in this step
				0,                  // request size of destination buffer, in chars
				nullptr, nullptr);  // unused
			if (utf8Length == 0)
			{
				// Conversion error
				// Return empty string
				return CStringA();
			}


			// Allocate destination buffer to store the resulting UTF-8 string
			CStringA utf8;
			char* const utf8Buffer = utf8.GetBuffer(utf8Length);
			ATLASSERT(utf8Buffer != nullptr);


			// Do the conversion from UTF-16 to UTF-8
			int result = ::WideCharToMultiByte(
				CP_UTF8,       // convert to UTF-8
				WC_ERR_INVALID_CHARS,              // conversion flags
				utf16.GetString(),  // source UTF-16 string
				utf16.GetLength(),  // length of source UTF-16 string, in WCHARs
				utf8Buffer,         // pointer to destination buffer
				utf8Length,         // size of destination buffer, in chars
				nullptr, nullptr);  // unused
			if (result == 0)
			{
				// Conversion error
				// Return empty string
				return CStringA();
			}


			// Don't forget to release internal CString buffer 
			// before returning the string to the caller
			utf8.ReleaseBufferSetLength(utf8Length);

			// Return resulting UTF-8 string
			return utf8;
		}

		static inline CStringW utf8to16(const CStringA& utf8)
		{
			// Special case of empty input string
			if (utf8.IsEmpty())
			{
				// Return empty string
				return CStringW();
			}

			// Get the length, in WCHARs, of the resulting UTF-16 string
			const int utf16Length = ::MultiByteToWideChar(
				CP_UTF8,       // source string is in UTF-8
				MB_ERR_INVALID_CHARS,              // conversion flags
				utf8.GetString(),   // source UTF-8 string
				utf8.GetLength(),   // length of source UTF-8 string, in chars
				nullptr,            // unused - no conversion done in this step
				0);                 // request size of destination buffer, in WCHARs
			if (utf16Length == 0)
			{
				// Conversion error
				// Return empty string
				return CStringW();
			}


			// Allocate destination buffer to store the resulting UTF-16 string
			CStringW utf16;
			WCHAR* const utf16Buffer = utf16.GetBuffer(utf16Length);
			ATLASSERT(utf16Buffer != nullptr);


			// Do the conversion from UTF-8 to UTF-16
			int result = ::MultiByteToWideChar(
				CP_UTF8,       // source string is in UTF-8
				MB_ERR_INVALID_CHARS,              // conversion flags
				utf8.GetString(),   // source UTF-8 string
				utf8.GetLength(),   // length of source UTF-8 string, in chars
				utf16Buffer,        // pointer to destination buffer
				utf16Length);       // size of destination buffer, in WCHARs  
			if (result == 0)
			{
				// Conversion error
				// Return empty string
				return CStringW();
			}

			// Don't forget to release internal CString buffer 
			// before returning the string to the caller
			utf16.ReleaseBufferSetLength(utf16Length);

			// Return resulting UTF-16 string
			return utf16;
		}

		static inline CString LoadStringResource(UINT nId) {
			CString strResource;
			(void)strResource.LoadString(nId);
			return strResource;
		}

		static inline BOOL GetKnownFolder(REFKNOWNFOLDERID rfid, CStringW& strVal)
		{
			BOOL bReturn{};
			LPWSTR lpszStrValue = nullptr;
			HRESULT hr = ::SHGetKnownFolderPath(rfid,
				KF_FLAG_DEFAULT, nullptr, &lpszStrValue);
			if (SUCCEEDED(hr))
			{
				strVal = lpszStrValue;
				bReturn = TRUE;
			}

			// it should be called whetever succeeded or not.
 			CoTaskMemFree(lpszStrValue);
			return bReturn;
		}

		//	unfortunately, LOGFONT structure does not contain any comparison operator,
		//	so we define our own comparison operators (==, != for now) to compare two LOGFONT structure objects.
		inline bool operator ==(const LOGFONT& lhs, const LOGFONT& rhs) {
			return lhs.lfHeight == rhs.lfHeight && !StrCmp(lhs.lfFaceName, rhs.lfFaceName) &&
				lhs.lfCharSet == rhs.lfCharSet && lhs.lfWeight == rhs.lfWeight && lhs.lfWidth == rhs.lfWidth &&
				lhs.lfItalic == rhs.lfItalic && lhs.lfStrikeOut == rhs.lfStrikeOut && lhs.lfUnderline == rhs.lfUnderline;
		}

		inline bool operator !=(const LOGFONT& lhs, const LOGFONT& rhs)
		{
			return !(lhs == rhs);
		}
#endif

		static inline std::string LoadFileContent(std::string_view strFilePath)
		{
			std::ifstream xmlFile(strFilePath.data());
			std::stringstream stream;
			stream << xmlFile.rdbuf();
			return stream.str();
		}

		/*
		*  Useful for standard map operations to detect whether the same value inserted or not.
		*  From cppreference.com:
		*	All member functions of all standard library specializations of this template are noexcept 
		*   except for the member functions of std::hash<std::optional>, std::hash<std::variant>, and 
		*   std::hash<std::unique_ptr>.
		*/
		template<typename T>
		struct KeyHasher
		{
			inline std::size_t operator()(const T& key) const
				noexcept(!std::is_same_v<T, std::optional<T>> &&
					!std::is_same_v<T, std::variant<T>> && !std::is_same_v<T, std::unique_ptr<T>>)
			{
				std::size_t h1 = std::hash<T>{}(key);
				return h1;
			}
		};

		/*
		*  MFC based CString string hash mechanism.
		*/
		template<typename T>
		class StringHash;

		template<>
		class StringHash<CString> {
		public:
			inline size_t operator()(const CString& s) const noexcept
			{
#ifdef UNICODE
				return std::hash<std::wstring>()((LPCWSTR)s);
#else
				return std::hash<std::string>()((LPCSTR)s);
#endif
			}
		};
	}
}