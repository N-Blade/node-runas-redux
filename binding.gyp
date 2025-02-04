{
    "target_defaults": {
        "win_delay_load_hook": "false",
        "conditions": [
            [
                'OS=="win"',
                {
                    "msvs_disabled_warnings": [
                        4530,  # C++ exception handler used, but unwind semantics are not enabled
                        4506,  # no definition for inline function
                    ],
                },
            ],
        ],
    },
    "targets": [
        {
            "target_name": "runas",
            "cflags!": ["-fno-exceptions"],
            "cflags_cc!": ["-fno-exceptions"],
            "msvs_settings": {
                "VCCLCompilerTool": {"ExceptionHandling": 1},
            },
            "sources": [
                "src/main.cc",
            ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
            ],
            "conditions": [
                [
                    'OS=="win"',
                    {
                        "sources": [
                            "src/runas_win.cc",
                        ],
                        "libraries": [
                            "-lole32.lib",
                            "-lshell32.lib",
                        ],
                    },
                ],
                [
                    'OS=="mac"',
                    {
                        "sources": [
                            "src/runas_darwin.cc",
                            "src/fork.cc",
                            "src/fork.h",
                        ],
                        "libraries": [
                            "$(SDKROOT)/System/Library/Frameworks/Security.framework",
                        ],
                        "defines": ["NAPI_DISABLE_CPP_EXCEPTIONS"],
                    },
                ],
                [
                    'OS not in ["mac", "win"]',
                    {
                        "sources": [
                            "src/runas_posix.cc",
                            "src/fork.cc",
                            "src/fork.h",
                        ],
                    },
                ],
            ],
        }
    ],
}
