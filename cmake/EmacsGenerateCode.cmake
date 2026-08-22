include_guard()

# ---- Find Emacs ----

find_program(EMACS_EXECUTABLE
    NAMES emacs emacs.exe
    HINTS
        ${EMACS_BINARY_DIR}/
        ${emacs_DIR}/../../../bin
        ${emacs_DIR}/bin
        # Standard system locations
        ${CMAKE_INSTALL_PREFIX}/bin
        /usr/local/bin
        /usr/bin
        # Windows
        "C:/Program Files/emacs/bin"
        $ENV{ProgramFiles}/emacs/bin
)

set(GENERATE_CODE_TARGET EMACS_GENERATE_CODE_TARGET)

if(NOT EMACS_EXECUTABLE)
    
    message(
        WARNING "Could not find emacs executable. Please install Emacs or set EMACS_EXECUTABLE."
    )
    message(STATUS "Fallback to shipped generated code.")

    add_custom_target(
         ${GENERATE_CODE_TARGET}
    )

else()
    
    message(STATUS "Found emacs at: ${EMACS_EXECUTABLE}")
    
    add_custom_target(
        ${GENERATE_CODE_TARGET}
        COMMAND "${EMACS_EXECUTABLE}"
        --batch --quick
	    --eval "(require 'org)"												
	    --eval "(org-babel-load-file \"${CMAKE_SOURCE_DIR}/org/InitSession.org\")"	
	    --eval "(wng/evaluate-module-org-files (wng/find-module-org-files \"${CMAKE_SOURCE_DIR}/Source\"))"
        COMMENT "Generating Source Code..."
        VERBATIM
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

endif()

