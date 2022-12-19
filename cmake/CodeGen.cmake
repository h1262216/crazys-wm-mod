find_package(Python3)

set(CODEGEN_BASE_PATH ${CMAKE_SOURCE_DIR}/src/codegen)

function(make_generated_header)
    set(MULTI_ARGS TARGET SCRIPT SOURCE SOURCE_FILE_TYPE ARGS OUTPUT BYPRODUCTS)
    cmake_parse_arguments(MGH "" "" "${MULTI_ARGS}" ${ARGN})
    set(SCRIPT_FILE "${CODEGEN_BASE_PATH}/${MGH_SCRIPT}")
    set(SOURCE "${RESOURCE_DIR}/${MGH_SOURCE}")
    set(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${MGH_OUTPUT}")
    file(GLOB SOURCE_FILES "${SOURCE}/*.${MGH_SOURCE_FILE_TYPE}")

    if(${Python3_Interpreter_FOUND})
        add_custom_command(
                COMMAND Python3::Interpreter "${SCRIPT_FILE}" "${OUTPUT}" "${SOURCE}" ${MGH_ARGS}
                DEPENDS "${SCRIPT_FILE}" ${SOURCE_FILES}
                OUTPUT "${OUTPUT}"
                BYPRODUCTS "${MGH_BYPRODUCTS}"
                COMMENT "Generating ${OUTPUT}."
        )
    else()
        message(WARNING "Could not find python3 interpreter! ${OUTPUT} cannot be regenerated and may be out of date")
    endif()
    add_custom_target("${MGH_TARGET}" DEPENDS "${OUTPUT}")
endfunction()

