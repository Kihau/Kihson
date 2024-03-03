project(
  'Kihson', 'c',
  version : '0.1',
  default_options : ['warning_level=3']
)

cc = meson.get_compiler('c')
math = cc.find_library('m', required : true)

exe = executable(
  'Kihson', 'src/main.c',
  'lib/kihson.c', 'lib/kihson_lexer.c', 'lib/kihson_parser.c', 'lib/kihson_string.c',
  install : true, dependencies: [ math ]
)

test('basic', exe)
