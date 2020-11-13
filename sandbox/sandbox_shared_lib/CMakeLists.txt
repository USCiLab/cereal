add_library(sandbox_vs_dll SHARED base.cpp derived.cpp)
target_link_libraries(sandbox_vs_dll ${CEREAL_THREAD_LIBS})
target_include_directories(sandbox_vs_dll PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)
