cmake_minimum_required(VERSION 3.8)
project(yxrobot_gui)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CUDA_TOOLKIT_ROOT_DIR /usr/local/cuda-12.4)
set(CUDA_NVCC_EXECUTABLE /usr/local/cuda-12.4/bin/nvcc)

# Force the use of the default system OpenCV to avoid the custom OpenCV's CUDA 12.3 requirement
set(OpenCV_DIR /usr/lib/x86_64-linux-gnu/cmake/opencv4)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(image_transport REQUIRED)
find_package(Eigen3 REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/src/mainwindow/ui)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Multimedia MultimediaWidgets)

file(GLOB PRO_FORM_DIR RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS src/mainwindow/ui/*.ui)
file(GLOB PRO_RESOURCES_DIR RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS src/mainwindow/resource/*.qrc)
file(GLOB_RECURSE PRO_INCLUDE_DIR RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS src/*.h src/*.hpp)
file(GLOB_RECURSE PRO_SOURCES_DIR RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS src/*.cpp)

qt5_wrap_ui(QT_UI_HPP ${PRO_FORM_DIR})
qt5_wrap_cpp(QT_MOC_HPP ${PRO_INCLUDE_DIR})
qt5_add_resources(QT_RESOURCES_CPP ${PRO_RESOURCES_DIR})

add_executable(${PROJECT_NAME}
  ${PRO_SOURCES_DIR}
  ${QT_RESOURCES_CPP}
  ${PRO_INCLUDE_DIR}
  ${QT_MOC_HPP}
  ${PRO_FORM_DIR}
)


target_link_libraries(${PROJECT_NAME}
  Qt5::Core
  Qt5::Widgets
  Qt5::Multimedia
  Qt5::MultimediaWidgets
)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>
  ${CMAKE_CURRENT_BINARY_DIR}
)

ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  sensor_msgs
  geometry_msgs
  nav_msgs
  std_msgs
  tf2
  tf2_ros
  tf2_geometry_msgs
  cv_bridge
  image_transport
)



install(TARGETS ${PROJECT_NAME}
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY resource/ ui/ include/ cmake_modules/ launch/
  DESTINATION share/${PROJECT_NAME}
)

ament_package()



