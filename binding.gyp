{
   'variables': {
      "prefers_libcpp":"<!(python -c \"import os;import platform;u=platform.uname();print((u[0] == 'Darwin' and int(u[2][0:2]) >= 13) and '-stdlib=libstdc++' not in os.environ.get('CXXFLAGS','') and '-mmacosx-version-min' not in os.environ.get('CXXFLAGS',''))\")"
    },
  "targets": [
    {
      "target_name": "msgprocessor",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [  "./bind/msgprocessor.cpp" ],
      'include_dirs': [ 
          '<!@(node -p "require(\'node-addon-api\').include")',
          '<!@(node -p "require(\'napi-thread-safe-callback\').include")',
          './utils',
          './bind'],
      'dependencies': [
          '<!(node -p "require(\'node-addon-api\').gyp")'],      
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],  
      'ldflags': [ '-Wl,-s'],     
      'conditions': [
        [ 'OS=="mac"', {
            'xcode_settings': 
            {
              'MACOSX_DEPLOYMENT_TARGET': '10.9',
              "CLANG_CXX_LIBRARY": "libc++",
              "GCC_ENABLE_CPP_RTTI": "YES",
              "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
              'OTHER_CPLUSPLUSFLAGS' : ['-std=c++14','-stdlib=libc++'],
              'OTHER_LDFLAGS': ['-stdlib=libc++'],
              'MACOSX_DEPLOYMENT_TARGET':'10.9',
              'OTHER_CPLUSPLUSFLAGS!': [ '-Os', '-O2', '-stdlib=libc++' ],
              "OTHER_CFLAGS": ["-stdlib=libc++"]
            },
        }],
      ],               
    }
  ]
}
