set(SOURCES
    main.cpp
    shader.cpp
    script_parser.cpp
    particle_system.cpp
    gui.cpp
    camera.cpp
)

add_executable(BlackHoleParticleSystem ${SOURCES})

set_target_properties(BlackHoleParticleSystem PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    OUTPUT_NAME "BlackHoleParticleSystem"
)

target_link_libraries(BlackHoleParticleSystem PRIVATE 
    glfw 
    glew_s 
    glm 
    opengl32
    imgui
)

target_include_directories(BlackHoleParticleSystem PRIVATE
    ../include
    ../third_party/glew/include
    ../third_party/glfw/include
    ../third_party/glm
    ../third_party/imgui
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_compile_features(BlackHoleParticleSystem PRIVATE cxx_std_17)

add_custom_command(TARGET BlackHoleParticleSystem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory 
        $<TARGET_FILE_DIR:BlackHoleParticleSystem>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:BlackHoleParticleSystem>/shaders
    COMMAND ${CMAKE_COMMAND} -E make_directory 
        $<TARGET_FILE_DIR:BlackHoleParticleSystem>/scripts
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/scripts
        $<TARGET_FILE_DIR:BlackHoleParticleSystem>/scripts
)