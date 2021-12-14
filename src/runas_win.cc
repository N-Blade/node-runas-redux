#include "runas.h"

#include <windows.h>

namespace runas {

std::wstring QuoteCmdArg(const std::wstring& arg) {
  if (arg.size() == 0)
    return arg;

  // No quotation needed.
  if (arg.find_first_of(L" \t\"") == std::wstring::npos)
    return arg;

  // No embedded double quotes or backlashes, just wrap quote marks around
  // the whole thing.
  if (arg.find_first_of(L"\"\\") == std::wstring::npos)
    return std::wstring(L"\"") + arg + L'"';

  // Expected input/output:
  //   input : hello"world
  //   output: "hello\"world"
  //   input : hello""world
  //   output: "hello\"\"world"
  //   input : hello\world
  //   output: hello\world
  //   input : hello\\world
  //   output: hello\\world
  //   input : hello\"world
  //   output: "hello\\\"world"
  //   input : hello\\"world
  //   output: "hello\\\\\"world"
  //   input : hello world\
  //   output: "hello world\"
  std::string quoted;
  bool quote_hit = true;
  for (size_t i = arg.size(); i > 0; --i) {
    quoted.push_back(arg[i - 1]);

    if (quote_hit && arg[i - 1] == '\\') {
      quoted.push_back('\\');
    } else if (arg[i - 1] == '"') {
      quote_hit = true;
      quoted.push_back('\\');
    } else {
      quote_hit = false;
    }
  }

  return std::wstring(L"\"") + std::wstring(quoted.rbegin(), quoted.rend()) + L'"';
}

bool Runas(const std::wstring& command,
           const std::vector<std::wstring>& args,
           const std::wstring& std_input,
           std::wstring* std_output,
           std::wstring* std_error,
           int options,
           int* exit_code) {
  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

  std::wstring parameters;
  for (size_t i = 0; i < args.size(); ++i)
    parameters += QuoteCmdArg(args[i]) + L' ';

  SHELLEXECUTEINFOW sei = { sizeof(sei) };
  sei.fMask = SEE_MASK_NOASYNC | SEE_MASK_NOCLOSEPROCESS;
  sei.lpVerb = (options & OPTION_ADMIN) ? std::wstring(L"runas").c_str() : std::wstring(L"open").c_str();
  sei.lpFile = command.c_str();
  sei.lpParameters = parameters.c_str();
  sei.nShow = SW_NORMAL;

  if (options & OPTION_HIDE)
    sei.nShow = SW_HIDE;

  if (::ShellExecuteExW(&sei) == FALSE || sei.hProcess == NULL)
    return false;

  // Wait for the process to complete.
  ::WaitForSingleObject(sei.hProcess, INFINITE);

  DWORD code;
  if (::GetExitCodeProcess(sei.hProcess, &code) == 0)
    return false;

  *exit_code = code;
  return true;
}

}  // namespace runas
