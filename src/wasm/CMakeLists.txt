find_package(Emscripten)

if(NOT EMSCRIPTEN)
    message(STATUS "Skipping wasm target: Emscripten not detected")
    return()
endif()

add_executable(tablut_wasm tablut_wasm.cpp)

target_link_libraries(tablut_wasm tablut adversarialSearch)

target_include_directories(tablut_wasm PRIVATE
    ${CMAKE_SOURCE_DIR}/src/ashtonTablut/include
)

target_compile_options(tablut_wasm PRIVATE -O3 -pthread)

target_compile_definitions(tablut_wasm PRIVATE ENABLE_METRICS)

target_link_options(tablut_wasm PRIVATE
    "SHELL:--bind"
    "SHELL:-sWASM=1"
    "SHELL:-sMODULARIZE=1"
    "SHELL:-sENVIRONMENT=web,worker"
    "SHELL:-sALLOW_MEMORY_GROWTH=1"
    "SHELL:-sUSE_PTHREADS=1"
    "SHELL:-sPTHREAD_POOL_SIZE=4"
    "SHELL:-sEXPORT_NAME=createTablutModule"
    "SHELL:-sASSERTIONS=1"
    "SHELL:-sSTACK_OVERFLOW_CHECK=2"
    "SHELL:-sDISABLE_EXCEPTION_CATCHING=0"
    "SHELL:-pthread"
)

target_compile_options(tablut_wasm PRIVATE -fexceptions)

set_target_properties(tablut_wasm PROPERTIES
    OUTPUT_NAME "tablut_wasm"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/wasm"
)
