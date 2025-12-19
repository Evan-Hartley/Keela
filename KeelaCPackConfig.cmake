set(CPACK_THREADS 0)
# show project license upon installation
set(CPACK_RESOURCE_FILE_LICENSE
        "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

# set up desktop links upon installation
set(CPACK_PACKAGE_EXECUTABLES "keela" "Keela")
set(CPACK_CREATE_DESKTOP_LINKS "keela")

# window-dressing stuff
set(CPACK_PACKAGE_VENDOR "Georgia Institute of Technology")
set(KEELA_RESOURCE_DIR "${PROJECT_SOURCE_DIR}/resources")
set(KEELA_ICON_FILE "${KEELA_RESOURCE_DIR}/KeelaIcon.ico")

#set(CPACK_CREATE_DESKTOP_LINKS keela)
# BEGIN NSIS specific configuration

set(CPACK_NSIS_MUI_ICON ${KEELA_ICON_FILE})
set(CPACK_NSIS_MUI_UNIICON ${KEELA_ICON_FILE})

#set(CPACK_NSIS_MODIFY_PATH ON)
# END NSIS configuration