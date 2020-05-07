cmake_minimum_required (VERSION 3.8)

function (header_directories RETURN_LIST)
  cmake_parse_arguments (
    HEADER_DIRECTORIES
    ""
    "ADDITIONAL_EXTENSIONS"
    ""
    ${ARGN}
    )

    set (EXTENSIONS *.h ${HEADER_DIRECTORIES_ADDITIONAL_EXTENSIONS})

    if (${INCLUDE_SOURCE_DIRECTORIES})
        set (EXTENSIONS ${EXTENSIONS} *.c *.cc)
    endif ()

    file (GLOB_RECURSE NEW_LIST ${EXTENSIONS})
    set (DIR_LIST "")

    foreach (FILE_PATH ${NEW_LIST})
        get_filename_component (DIR_PATH "${FILE_PATH}" PATH)
        string (FIND ${DIR_PATH} "mcuapp.arm" SUBSTR)
        if (${SUBSTR} EQUAL "-1")
            set (DIR_LIST "${DIR_LIST}" "${DIR_PATH}")
        endif ()
    endforeach ()

    list (REMOVE_DUPLICATES DIR_LIST)
    list (SORT DIR_LIST)

    if (REVERSE_INCLUDES_DIRS)
        list (REVERSE DIR_LIST)
    endif ()

    set (${RETURN_LIST} "${DIR_LIST}" PARENT_SCOPE)
endfunction ()
