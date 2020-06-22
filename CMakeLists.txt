cmake_minimum_required(VERSION 3.16)
project(PhotoGraph)
add_executable(PhotoGraph)
set_target_properties(PhotoGraph PROPERTIES CXX_STANDARD 20)
target_compile_options(PhotoGraph PRIVATE -Wpedantic -Werror -Wextra)
target_compile_options(PhotoGraph PRIVATE $<$<CONFIG:RELEASE>:-Ofast;-march=native;-Wno-error=unused-parameter>)
target_compile_options(PhotoGraph PRIVATE $<$<CONFIG:DEBUG>:-Og;-g>)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(PhotoGraph PRIVATE $<$<CONFIG:DEBUG>:-fstandalone-debug>)
endif()

# QNodeGraph
find_package(QNodeGraph REQUIRED)
target_link_libraries(PhotoGraph PUBLIC QNodeGraph::QNodeGraph)

# ImageGraph
find_package(ImageGraph REQUIRED)
target_link_libraries(PhotoGraph PUBLIC ImageGraph::ImageGraph)

# Abseil
find_package(absl REQUIRED)
target_link_libraries(PhotoGraph PUBLIC absl::flat_hash_map absl::flat_hash_set)

set(SOURCES
    main.cpp
    ComputationButton.cpp
    ConfigDialog.cpp
    ConfigPages.cpp
    ImageGraphView.cpp
    MainWindow.cpp
    NodeHelp.cpp
    TestImageGraph.cpp
    TimeIntervals.cpp
    nodes/creators/Addition.cpp
    nodes/creators/Clamp.cpp
    nodes/creators/Convert.cpp
    nodes/creators/Division.cpp
    nodes/creators/Gamma.cpp
    nodes/creators/GaussianBlur.cpp
    nodes/creators/Linear.cpp
    nodes/creators/Multiplication.cpp
    nodes/creators/Resize.cpp
    nodes/creators/Subtraction.cpp
    nodes/ImageClampNode.cpp
    nodes/ImageConvertNode.cpp
    nodes/ImageGammaNode.cpp
    nodes/ImageGaussianNode.cpp
    nodes/ImageInputNode.cpp
    nodes/ImageLinearNode.cpp
    nodes/ImageOutputNode.cpp
    nodes/ImageResizeNode.cpp)
qt5_wrap_cpp(
  SOURCES
  ConfigDialog.hpp
  ConfigPages.hpp
  ComputationButton.hpp
  ImageGraphView.hpp
  JsonWidgets.hpp
  MagicTricks.hpp
  MainWindow.hpp
  ParameterManager.hpp
  TypeComboBox.hpp
  TypedLineEdit.hpp
  nodes/ImageClampNode.hpp
  nodes/ImageConvertNode.hpp
  nodes/ImageGammaNode.hpp
  nodes/ImageGaussianNode.hpp
  nodes/ImageInputNode.hpp
  nodes/ImageLinearNode.hpp
  nodes/ImageOutputNode.hpp
  nodes/ImageResizeNode.hpp
  TARGET
  PhotoGraph)
target_sources(PhotoGraph PRIVATE ${SOURCES})
