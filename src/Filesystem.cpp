#include <Filesystem.hpp>
#include <StringUtil.hpp>
#include <windows.h>
#include <wincrypt.h>
#include <cassert>
#pragma comment(lib, "Advapi32.lib")

namespace
{
  int acquire_crypt_handle(HCRYPTPROV& handle)
  {
    if (::CryptAcquireContextW(&handle, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
      return 0;

    int errval = ::GetLastError();
    if (errval != NTE_BAD_KEYSET)
      return errval;

    if (::CryptAcquireContextW(&handle, 0, 0, PROV_RSA_FULL, CRYPT_NEWKEYSET | CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
      return 0;

    errval = ::GetLastError();
    // Another thread could have attempted to create the keyset at the same time.
    if (errval != NTE_EXISTS)
      return errval;

    if (::CryptAcquireContextW(&handle, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
      return 0;

    return ::GetLastError();
  }

  void system_crypt_random(void* buf, std::size_t len)
  {
    HCRYPTPROV handle;
    int errval = acquire_crypt_handle(handle);

    if (!errval)
    {
      BOOL gen_ok = ::CryptGenRandom(handle, (DWORD)len, static_cast<unsigned char*>(buf));
      if (!gen_ok)
        errval = ::GetLastError();
      ::CryptReleaseContext(handle, 0);
    }

    if (!errval)
      return;

    std::system_error(errval, Filesystem::ErrorCategories::SystemCategory(), "Filesystem::unique_path");
  }
} // unnamed namespace

namespace std
{
  namespace experimental
  {
    namespace filesystem
    {
      path unique_path(const path& model)
      {
        std::wstring s (model.wstring()); // std::string ng for MBCS encoded POSIX
        const wchar_t hex[] = L"0123456789abcdef";
        char ran[] = "123456789abcdef"; // init to avoid clang static analyzer message
                                        // see ticket #8954
        assert(sizeof(ran) == 16);
        const int max_nibbles = 2 * sizeof(ran); // 4-bits per nibble

        int nibbles_used = max_nibbles;
        for (std::wstring::size_type i = 0; i < s.size(); ++i)
          if (s[i] == L'%') // digit request
          {
            if (nibbles_used == max_nibbles)
            {
              system_crypt_random(ran, sizeof(ran));
              nibbles_used = 0;
            }
            int c = ran[nibbles_used / 2];
            c >>= 4 * (nibbles_used++ & 1); // if odd, shift right 1 nibble
            s[i] = hex[c & 0xf]; // convert to hex digit and replace
          }

        return s;
      }

      path SystemComplete(const path& p)
      {
        if (p.empty())
          return p;

        return std::experimental::filesystem::system_complete(p);
      }

      std::time_t LastWriteTime(const path& p)
      {
        auto lastWriteTime = std::experimental::filesystem::last_write_time(p);
        return decltype(lastWriteTime)::clock::to_time_t(lastWriteTime);
      }

      std::string Path::string() const
      {
        // return path::u8string(); is slow because it initializes wstring_convert every time it is called.
        return StringUtil::narrow(this->native());
      }
    }
  }
}
