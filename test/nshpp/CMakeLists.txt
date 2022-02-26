include(FixedString)

nsh_add_tool(nshpp src/main.cpp src/meta/TypeList.test.cpp src/StaticBinding.test.cpp)
target_include_directories(nshpp PUBLIC include)
target_compile_features(nshpp PUBLIC cxx_std_20)