ENABLE_TESTING()
ADD_TEST(test_call1 ${FreeMat_Loc} "-e" "-nogui" "-t" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/functions" "-f" "wrap_test('test_call1')")
ADD_TEST(test_call2 ${FreeMat_Loc} "-e" "-nogui" "-t" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/functions" "-f" "wrap_test('test_call2')")
ADD_TEST(test_call3 ${FreeMat_Loc} "-e" "-nogui" "-t" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/functions" "-f" "wrap_test('test_call3')")
ADD_TEST(test_call4 ${FreeMat_Loc} "-e" "-nogui" "-t" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/functions" "-f" "wrap_test('test_call4')")
ADD_TEST(test_fptr1 ${FreeMat_Loc} "-e" "-nogui" "-t" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/functions" "-f" "wrap_test('test_fptr1')")
ADD_TEST(test_fcall1 ${FreeMat_Loc} "-e" "-nogui" "-t" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/functions" "-f" "wrap_test('test_fcall1')")
ADD_TEST(test_fcall2 ${FreeMat_Loc} "-e" "-nogui" "-t" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/functions" "-f" "wrap_test('test_fcall2')")
ADD_TEST(test_nargin1 ${FreeMat_Loc} "-e" "-nogui" "-t" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/functions" "-f" "wrap_test('test_nargin1')")
ADD_TEST(test_nargin2 ${FreeMat_Loc} "-e" "-nogui" "-t" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/functions" "-f" "wrap_test('test_nargin2')")
