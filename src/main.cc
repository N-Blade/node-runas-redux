#include "napi.h"
#include "uv.h"
#include "runas.h"

using namespace Napi;

Napi::Value Runas(const Napi::CallbackInfo &info)
{
  auto env = info.Env();
  if (!info[0].IsString() || !info[1].IsArray() || !info[2].IsObject())
  {
    Napi::TypeError::New(env, "Bad argument").ThrowAsJavaScriptException();
    return env.Null();
  }

  std::string command(info[0].As<Napi::String>().Utf8Value().c_str());
  std::vector<std::string> c_args;

  Napi::Array v_args = info[1].As<Napi::Array>();
  uint32_t length = v_args.Length();

  c_args.reserve(length);
  for (uint32_t i = 0; i < length; ++i)
  {
    std::string arg((v_args).Get(i).As<Napi::String>().Utf8Value().c_str());
    c_args.push_back(arg);
  }

  Napi::Object v_options = info[2].As<Napi::Object>();
  int options = runas::OPTION_NONE;

  auto v_hide = v_options.Get("hide");
  if (v_hide.IsBoolean() && v_hide.As<Napi::Boolean>().Value())
    options |= runas::OPTION_HIDE;

  auto v_admin = v_options.Get("admin");
  if (v_admin.IsBoolean() && v_admin.As<Napi::Boolean>().Value())
    options |= runas::OPTION_ADMIN;

  auto v_stdin = v_options.Get("stdin");
  std::string std_input;
  if (v_stdin.IsString())
    std_input = v_stdin.As<Napi::String>().Utf8Value().c_str();

  auto v_catchOutput = v_options.Get("catchOutput");
  std::string std_output, std_error;
  bool catch_output = false;
  if (v_catchOutput.IsBoolean()) {
    catch_output = v_catchOutput.As<Napi::Boolean>().Value();
    if (catch_output)
      options |= runas::OPTION_CATCH_OUTPUT;
  }

  int code = -1;
  runas::Runas(command, c_args, std_input, &std_output, &std_error, options, &code);

  if (catch_output)
  {
    Napi::Object result = Napi::Object::New(env);
    (result).Set(Napi::String::New(env, "exitCode"),
                 Napi::Number::New(env, code));
    (result).Set(Napi::String::New(env, "stdout"),
                 Napi::String::New(env, std_output));
    (result).Set(Napi::String::New(env, "stderr"),
                 Napi::String::New(env, std_error));
    return result;
  }
  else
  {
    return Napi::Number::New(env, code);
  }
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports.Set("runas", Napi::Function::New(env, Runas));
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
