find_package(Doxygen)
if(DOXYGEN_FOUND)

  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/doxygen.in" "${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg" @ONLY)
  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.."
    COMMENT "Generating API documentation with Doxygen" VERBATIM
    )

  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../scripts/updatedoc.in" "${CMAKE_CURRENT_BINARY_DIR}/updatedoc.sh" @ONLY)
  add_custom_target(update-doc
    COMMAND "${CMAKE_CURRENT_BINARY_DIR}/updatedoc.sh"
    DEPENDS doc
    COMMENT "Copying documentation to gh-pages branch" VERBATIM
    )

endif(DOXYGEN_FOUND)