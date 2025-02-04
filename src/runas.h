#ifndef SRC_RUNAS_H_
#define SRC_RUNAS_H_

#include <string>
#include <vector>

namespace runas {

enum Options {
  OPTION_NONE  = 0,
  // Hide the command's window.
  OPTION_HIDE  = 1 << 1,
  // Run as administrator.
  OPTION_ADMIN = 1 << 2,
  // Catch the output.
  OPTION_CATCH_OUTPUT = 1 << 3,
};

bool Runas(const std::wstring& command,
           const std::vector<std::wstring>& args,
           const std::wstring& std_input,
           std::wstring* std_output,
           std::wstring* std_error,
           int options,
           int* exit_code);

bool Runas(const std::string& command,
           const std::vector<std::string>& args,
           const std::string& std_input,
           std::string* std_output,
           std::string* std_error,
           int options,
           int* exit_code);
}  // namespace runas


#endif  // SRC_RUNAS_H_
