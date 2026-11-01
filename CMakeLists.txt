cmake_minimum_required (VERSION 3.10)

option(BUILD_TESTS "build tests" OFF)
option(ENABLE_CLIENT "Enable client code to be built" ON)
option(LINK_ABSL "explicitely link abseil components" ON)

project (toffy_oatpp)

set (toffy_oatpp_VERSION_MAJOR 0)
set (toffy_oatpp_VERSION_MINOR 1)
set (toffy_oatpp_VERSION_REVISION 1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE OFF)


################################################################################
# Cmake module definitions
################################################################################

set(${PROJECT}_LIBRARIES ${PROJECT})

################################################################################
# Cmake modules
################################################################################

#Including local extra cmake modules
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

#Function for generation of export macros for libraries
include(GenerateExportHeader) #(toffy_export.h - TOFFY_EXPORT)

# Include module with fuction 'write_basic_package_version_file' for ToffyConfig.cmake
include(CMakePackageConfigHelpers)

# Build dir for generated config, export and package files
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/generated")


################################################################################
# Install options
################################################################################
MESSAGE(STATUS "CMAKE_INSTALL_PREFIX: " ${CMAKE_INSTALL_PREFIX})

set(INSTALL_LIB_DIR lib/ CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin/ CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include/${PROJECT_NAME} CACHE PATH "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR cmake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/cmake/${PROJECT_NAME})
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

################################################################################
#Version stuff
################################################################################

# Make a version file containing the current version from git.
include(GetGitRevisionDescription)
git_describe(VERSION --tag --long)

##parse the version information into pieces.
string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" PROJECT_VERSION_MAJOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" PROJECT_VERSION_MINOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" PROJECT_VERSION_PATCH "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" PROJECT_VERSION_TWEAK "${VERSION}")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(PROJECT_DESCRIPTION "Toffy 3D computer vision framework")

set(PROJECT_VERSION_FULL "${PROJECT_VERSION}${PROJECT_VERSION_TWEAK}")

message (STATUS "version: " ${VERSION} )
message (STATUS ${PROJECT_NAME}" version: " ${PROJECT_VERSION_FULL} )

# Generate Config file y_config.h
set(config_h_in "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}_config.h.in")
set(config_h "${generated_dir}/include/${PROJECT_NAME}/${PROJECT_NAME}_config.h")
configure_file("${config_h_in}" "${config_h}")

list(APPEND headers ${config_h})
install(
   FILES ${config_h}
   DESTINATION "${INSTALL_INCLUDE_DIR}/common"
)

################################################################################
# Install Stuff
################################################################################

#######################################
# Config cmake
#######################################
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake") #output
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake") #output

#Targets name
#   * ${INSTALL_CMAKE_DIR}/${targets_export_name}.cmake
set(targets_export_name "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

install(
    EXPORT "${targets_export_name}"
    NAMESPACE "${namespace}"
    DESTINATION "${INSTALL_CMAKE_DIR}/"
)

# Set version required to config
write_basic_package_version_file(
    "${version_config}"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# '<PROJECT-NAME>Config.cmake' is generated further down, after the find_package
# calls: the template records the dependency versions this library was built against,
# and those variables do not exist yet at this point in the file.

# Instal config
#   * <prefix>/lib/cmake/toffy/toffyConfig.cmake --> ${project_config}
#   * <prefix>/lib/cmake/toffy/toffyConfigVersion.cmake --> ${version_config}
install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${INSTALL_CMAKE_DIR}"
)

install(
    EXPORT "${targets_export_name}"
    NAMESPACE "${namespace}"
    DESTINATION "${INSTALL_CMAKE_DIR}/"
)


INSTALL( PROGRAMS ${binaries}
    DESTINATION ${INSTALL_BIN_DIR}
)

################################################################################
#
################################################################################

#
# needed for architecture-specific extensions like gpio trigger etc.
#
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
    add_definitions(-DLINUX=1 )
endif()

find_package(PCL REQUIRED COMPONENTS common visualization filters sample_consensus segmentation features surface )
if (PCL_FOUND)
    include_directories(${PCL_INCLUDE_DIRS})
    add_definitions(${PCL_DEFINITIONS})
    list(APPEND LIBS ${PCL_LIBRARIES})
    message (STATUS "PCL_INCLUDE_DIRS: " ${PCL_INCLUDE_DIRS})
    message (STATUS "PCL_LIBRARIES: " ${PCL_LIBRARIES})
    add_definitions(-DPCL_FOUND)
    include_directories( ${PCL_INCLUDE_DIRS} )
    message (STATUS "PCL_FOUND: " ${PCL_FOUND})
else()
    set(WITHOUT_PCL 1)
endif()


find_package( OpenCV COMPONENTS core imgproc highgui calib3d videoio REQUIRED )
if (OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS})
    add_definitions(${OpenCV_DEFINITIONS})
    list(APPEND LIBS ${OpenCV_LIBRARIES})
    message (STATUS "OpenCV_INCLUDE_DIRS: " ${OpenCV_INCLUDE_DIRS})
    message (STATUS "OpenCV_LIBRARIES: " ${OpenCV_LIBRARIES})
    add_definitions(-DOpenCV_FOUND)
    message (STATUS "OpenCV_FOUND: " ${OpenCV_FOUND})
endif()

find_package(toffy 2.0 REQUIRED )
if (toffy_FOUND)
    include_directories(${toffy_INCLUDE_DIRS})
    # toffy__DEFINITIONS (double underscore) expanded to nothing, so none of this ever
    # reached the target. What it carries, from toffyConfig.cmake:
    #   -DEIGEN_DONT_VECTORIZE -DEIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
    #   -DEIGEN_SUPPORT_STAGE=99 -DBOOST_ALL_DYN_LINK -DBOOST_ALL_NO_LIB
    # The Eigen flags change object layout, so this was an ABI mismatch between
    # libtoffy_oatpp.so and every other component in a process that links both it and
    # libtoffy -- boxes applies them, toffy_oatpp did not.
    add_definitions(${toffy_DEFINITIONS})
    list(APPEND LIBS ${toffy_LIBRARIES})
endif()

################################################################################
# Is BTA in the toffy we are building against?  (P1-13)
#
# The BTA adapter controller includes <toffy/bta/bta.hpp> and <toffy/bta/
# BtaWrapper.hpp>, and toffy installs those only when it was built with the
# proprietary SDK — so against a BTA-less toffy this library did not compile at
# all, which is the first thing toffy's own consumer gate found (P0-2). The flag
# comes from toffyConfig.cmake (`toffy_WITH_BTA`, 0 or 1, always defined there);
# `find_package(toffy 2.0 REQUIRED COMPONENTS BTA)` is the alternative for a build
# that cannot work without it, and fails with CMake's own message.
#
# What the flag does to the API: the /api/bta* endpoints are **absent**, not
# 503-ing. That is not a shortcut — webApi.cpp already refuses to register the
# controller when there is no bta filter in the bank (`if (systemState->bta)`), so
# "no BTA hardware, therefore no BTA endpoints" is the existing contract and
# compile-time absence is the same statement one level earlier. An endpoint that
# answers 503 would advertise in swagger a camera this build can never talk to.
################################################################################
if (DEFINED toffy_WITH_BTA)
    set(TOFFYOATPP_WITH_BTA ${toffy_WITH_BTA})
    set(_bta_provenance "toffyConfig.cmake: toffy_WITH_BTA=${toffy_WITH_BTA}")
else()
    # A toffy older than the flag. Undefined must NOT read as "no": that would
    # silently drop the BTA endpoints from a build whose toffy has them, and a
    # silently missing feature is the bug this whole item is about. Probe instead,
    # and say that we probed. The loop is not decoration: toffy_INCLUDE_DIRS is a
    # list (and in practice carries an empty leading element), and `if(EXISTS)` on
    # a list is a question that has no answer — so test each entry and report the
    # one that decided it.
    set(_bta_hdr "")
    foreach(_toffy_inc IN LISTS toffy_INCLUDE_DIRS)
        if (_toffy_inc AND EXISTS "${_toffy_inc}/toffy/bta/bta.hpp")
            set(_bta_hdr "${_toffy_inc}/toffy/bta/bta.hpp")
            break()
        endif()
    endforeach()
    if (_bta_hdr)
        set(TOFFYOATPP_WITH_BTA 1)
    else()
        set(TOFFYOATPP_WITH_BTA 0)
    endif()
    set(_bta_provenance "this toffy does not export toffy_WITH_BTA — probed (${_bta_hdr})")
endif()
message(STATUS "BTA adapter endpoints: ${TOFFYOATPP_WITH_BTA} (${_bta_provenance})")


# Boost
# no `thread` component: nothing here uses boost::thread or boost::mutex -- the
# concurrency is std::thread/std::condition_variable, and toffy dropped
# <boost/thread.hpp> from its installed headers in P2-7. The component was only ever
# pulling libboost_thread into the link line.
find_package(Boost COMPONENTS log_setup log program_options system
        filesystem date_time serialization chrono REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
list(APPEND LIBS ${Boost_LIBRARIES})

add_definitions(-DBOOST_ALL_DYN_LINK )


# OAT++
find_package(oatpp          1.4.0 REQUIRED)
find_package(oatpp-swagger  1.4.0 REQUIRED)

include_directories("${oatpp-swagger_INCLUDE_DIRS}" ${oatpp_INCLUDE_DIRS})
link_directories(${oatpp_LIBRARIES_DIRS})
link_directories(${oatpp-swagger_LIBRARIES_DIRS})

list(APPEND LIBS oatpp::oatpp  oatpp::oatpp-test oatpp::oatpp-swagger)
message("OAT INC " ${oatpp_INCLUDE_DIRS})
message("OAT INC " ${oatpp-swagger_INCLUDE_DIRS})

add_definitions(
        ## define path to swagger-ui static resources folder
        -DOATPP_SWAGGER_RES_PATH="${oatpp-swagger_INCLUDE_DIRS}/../bin/oatpp-swagger/res"
)

# Now that the dependency versions are known, generate the package config from them.
#   * toffy_oatpp's config used to declare no dependency at all, so every consumer had
#     to repeat find_package(toffy <ver>) by hand -- and drift out of step with it.
#   * the version recorded is the one this library was compiled against, which with
#     toffy's full-version SOVERSION is the only version it can link to.
configure_package_config_file("cmake/Config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${INSTALL_CMAKE_DIR}"
)

################################################################################

# (a `list(APPEND DEFINITIONS -DEIGEN_DONT_VECTORIZE …)` used to sit here: never passed
#  to add_definitions, never read by anything. The same flags now arrive through
#  ${toffy_DEFINITIONS}, which is the one place they should be set.)

# BEFORE, and that is not cosmetic. toffy and toffy_oatpp install into the same
# prefix, and `${toffy_INCLUDE_DIRS}` above is that prefix — so a plain
# include_directories() here, at the end of the file, puts the *previously
# installed* copy of this project's own public headers ahead of the ones being
# edited. Measured: an edit to include/toffy_oatpp/webapi/appComponent.hpp was
# silently ignored — `flags.make` lists `-I <stage>/include` before
# `-I <src>/include`, the compile used the stale header, and the behaviour the
# edit was supposed to change did not change, with no warning anywhere. A
# project has to see its own headers first.
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/api ${CMAKE_CURRENT_SOURCE_DIR}/include )

if(CMAKE_BUILD_TYPE MATCHES Release)
    message("RELEASE BUILD")
    add_definitions(-O2 -funroll-loops )
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("DEBUG BUILD")
    add_definitions(-Wall -ggdb -O0 )
    # add_definitions( -Weffc++ )
endif()


add_subdirectory(src)

add_subdirectory(app)

add_subdirectory(tests)

# Installed headers. The BTA adapter controller header is excluded from a BTA-less
# install: it includes toffy/bta/*.hpp, which such a toffy does not ship, so
# installing it would hand a consumer a header it cannot compile — the same shape
# as toffy's own P0-5 (public headers pointing at a toffy that is not there),
# caught here before it ships rather than by a user. The DTO stays: it is pure
# oatpp and compiles either way.
if (TOFFYOATPP_WITH_BTA)
    install(DIRECTORY "include/" DESTINATION "include/")
else()
    install(DIRECTORY "include/" DESTINATION "include/"
            PATTERN "btaAdapterController.hpp" EXCLUDE)
endif()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${targets_export_name} 
    LIBRARY
        DESTINATION "${INSTALL_LIB_DIR}" COMPONENT library INCLUDES NAMELINK_SKIP
    ARCHIVE
        DESTINATION lib
    RUNTIME
        DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
    INCLUDES
        DESTINATION "${INSTALL_INCLUDE_DIR}"
    #We create only the include folder. Our headers have a specfic folder hierarchy
    PUBLIC_HEADER
        DESTINATION "${INSTALL_INCLUDE_DIR}" COMPONENT headers
)
