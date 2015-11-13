cmake_minimum_required(VERSION 2.8.8)
project(autoreg)

set (autoreg_VERSION_MAJOR 0)
set (autoreg_VERSION_MINOR 1)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/include")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

if(CMAKE_COMPILER_IS_GNUCXX)
    if (NOT WIN32)
        add_definitions(-fPIC)
    endif()
    add_definitions(-O3)
    add_definitions(-g)
    add_definitions(-std=c++14)
    add_definitions(-Wall)
    add_definitions(-Wno-unused-function)
    add_definitions(-Wno-unused-local-typedefs)
    add_definitions(-Wno-maybe-uninitialized)
    add_definitions(-Wno-deprecated-declarations)
    add_definitions(-DWITH_CEREAL -DWITH_IFCOPENSHELL -DWITH_PCL -DWITH_PRIMITIVE_DETECTION -DWITH_OPENMESH -DWITH_E57PCL)
endif()

# mandatory
find_package(Eigen)
#find_package(OpenCV COMPONENTS core imgproc highgui)
find_package(Boost COMPONENTS system filesystem program_options)

# optional
#find_package(Cereal)
#find_package(OpenCascade)
#find_package(IfcOpenShell)
#find_package(OpenMesh)
#find_package(PCL COMPONENTS common io)
#find_package(PrimitiveDetection)
#find_package(E57PCL)
find_package(October)
find_package(DuraarkRDF)

file (GLOB_RECURSE obj RELATIVE "${PROJECT_SOURCE_DIR}" "src/*.cpp")
if (OCTOBER_FOUND AND DURAARKRDF_FOUND)
    include_directories(${EIGEN_INCLUDE_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})
    include_directories(${OCTOBER_INCLUDE_DIRS})
    include_directories(${DURAARKRDF_INCLUDE_DIRS})

    #add_library(october SHARED ${obj})
    #target_link_libraries(october ${OpenCV_LIBRARIES} ${Boost_LIBRARIES} ${OPENCASCADE_LIBRARIES} ${IFCOPENSHELL_LIBRARIES} ${OPENMESH_LIBRARIES} ${PCL_LIBRARIES} ${E57PCL_LIBRARIES} ${PRIMITIVE_DETECTION_LIBRARIES} "dl")

    add_executable(autoreg ${obj})
    
    target_link_libraries(autoreg ${Boost_LIBRARIES} ${OCTOBER_LIBRARIES} ${DURAARKRDF_LIBRARIES})
    install (TARGETS autoreg DESTINATION bin)
endif()
