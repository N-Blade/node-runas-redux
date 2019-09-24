#include "nan.h"
using namespace v8;

#include "runas.h"

namespace {

inline
bool GetProperty(Local<Object> obj, const char* key, Local<Value>* value) {
  return Nan::Get(obj, Nan::New<String>(key).ToLocalChecked()).ToLocal(value);
}

void Runas(const Nan::FunctionCallbackInfo<Value>& info) {
  if (!info[0]->IsString() || !info[1]->IsArray() || !info[2]->IsObject()) {
    Nan::ThrowTypeError("Bad argument");
    return;
  }

  std::string command(*Nan::Utf8String(info[0]));
  std::vector<std::string> c_args;

  Local<Array> v_args = Local<Array>::Cast(info[1]);
  uint32_t length = v_args->Length();

  c_args.reserve(length);
  for (uint32_t i = 0; i < length; ++i) {
    std::string arg(*Nan::Utf8String(Nan::Get(v_args, i).ToLocalChecked()));
    c_args.push_back(arg);
  }

  Local<Value> v_value;
  Local<Object> v_options = Nan::To<Object>(info[2]).ToLocalChecked();
  int options = runas::OPTION_NONE;
  if (GetProperty(v_options, "hide", &v_value) && Nan::To<bool>(v_value).FromJust())
    options |= runas::OPTION_HIDE;
  if (GetProperty(v_options, "admin", &v_value) && Nan::To<bool>(v_value).FromJust())
    options |= runas::OPTION_ADMIN;

  std::string std_input;
  if (GetProperty(v_options, "stdin", &v_value) && v_value->IsString())
    std_input = *Nan::Utf8String(v_value);

  std::string std_output, std_error;
  bool catch_output = GetProperty(v_options, "catchOutput", &v_value) &&
                      Nan::To<bool>(v_value).FromJust();
  if (catch_output)
    options |= runas::OPTION_CATCH_OUTPUT;

  int code = -1;
  runas::Runas(command, c_args, std_input, &std_output, &std_error, options,
               &code);

  if (catch_output) {
    Local<Object> result = Nan::New<Object>();
    Nan::Set(result,
             Nan::New<String>("exitCode").ToLocalChecked(),
             Nan::New<Integer>(code));
    Nan::Set(result,
             Nan::New<String>("stdout").ToLocalChecked(),
             Nan::New<String>(std_output).ToLocalChecked());
    Nan::Set(result,
             Nan::New<String>("stderr").ToLocalChecked(),
             Nan::New<String>(std_error).ToLocalChecked());
    info.GetReturnValue().Set(result);
  } else {
    info.GetReturnValue().Set(Nan::New<Integer>(code));
  }
}

}  // namespace


NAN_MODULE_INIT(Init) {
  Export(target, "runas", Runas);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)
