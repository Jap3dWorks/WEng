include_guard()

find_package(slang REQUIRED)

get_filename_component(SLANG_ROOT_DIR "${slang_DIR}" DIRECTORY)
find_program(SLANGC_EXECUTABLE
    NAMES slangc slang-compiler
    HINTS 
        ${SLANG_ROOT_DIR}/bin
        ${Slang_DIR}/bin
        /usr/local/bin
    REQUIRED
)

function(add_slang_shader_target TARGET)
    cmake_parse_arguments(
        PARSE_ARGV 1
        ARG
        ""
        "SHADERS_DIR;OUTPUT_DIR"
        "ENTRY_POINT_MAP"
    )

    if(NOT ARG_OUTPUT_DIR)
        set(ARG_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Content/Shaders")
    endif()

    if (NOT ARG_SHADERS_DIR)
        set(ARG_SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Content/Shaders")
    endif()
    
    message("Compile shaders from ${ARG_SHADERS_DIR}.")
    
    file(GLOB SHADER_SOURCES ${ARG_SHADERS_DIR}/*.slang)

    message("Collected shaders: ${SHADER_SOURCES}.")

    file(MAKE_DIRECTORY ${OUTPUT_DIR})

    set(ALL_SPV_FILES)

    foreach(SHADER_SOURCE ${SHADER_SOURCES})

        get_filename_component(SHADER_NAME ${SHADER_SOURCE} NAME_WLE)
        
        set(PIPELINE_SPV "${ARG_OUTPUT_DIR}/${SHADER_NAME}.spv")

        if(${SHADER_NAME} MATCHES ".*.comp.slang$")
            set(ENTRY_FLAGS "-entry" "csMain")
            set(SHADER_TYPE "compute")
        elseif(${SHADER_NAME} MATCHES ".*.graphic.slang$")
            set(ENTRY_FLAGS "-entry" "vsMain" "-entry" "fsMain")
            set(SHADER_TYPE "graphics")
        else()
            set(ENTRY_FLAGS "-entry" "vsMain" "-entry" "fsMain")
            set(SHADER_TYPE "unknown")
        endif()

        message("START Compile ${SHADER_SOURCE} to ${PIPELINE_SPV} with entries ${ENTRY_FLAGS}")

        add_custom_command(
            OUTPUT ${PIPELINE_SPV}
            COMMAND ${SLANGC_EXECUTABLE}
                ${SHADER_SOURCE}
                -target spirv
                -profile spirv_1_4
                -emit-spirv-directly
                ${ENTRY_FLAGS}
                -o ${PIPELINE_SPV}
            DEPENDS ${SHADER_SOURCE}
            COMMENT "Compiling ${SHADER_TYPE} pipeline ${SHADER_NAME}.spv"
            VERBATIM
        )

        list(APPEND ALL_SPV_FILES ${PIPELINE_SPV})
        
    endforeach()
    
    add_custom_target(${TARGET} 
        DEPENDS ${ALL_SPV_FILES}
    )
    
endfunction()
