
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/test_optimisation)

# all source files
set(SRC ${SRCROOT}/Test.cpp)

# define the test target
sfml_add_example(test GUI_APP
                 SOURCES ${SRC}
                 DEPENDS sfml-window sfml-graphics)
