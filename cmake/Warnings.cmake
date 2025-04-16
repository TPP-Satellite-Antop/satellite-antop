function(target_set_warnings TARGET)
    set(WARNINGS
            -Wall
            -Wextra
            -Wpedantic
            -Werror)
    target_compile_options(${TARGET} PRIVATE ${WARNINGS})
    message(STATUS "Compiler warnings: ${WARNINGS}")
endfunction(target_set_warnings)

