cmake_minimum_required(VERSION 3.12)
project(test)


find_package(ABY QUIET)
 if(ABY_FOUND)
 	message(STATUS "Found ABY")
 elseif (NOT ABY_FOUND AND NOT TARGET ABY::aby)
 	message("ABY was not found: add ABY subdirectory")
 	add_subdirectory(/home/ABY)
 endif()


add_executable(server server.cpp common/pptraclus.cpp)
add_executable(client client.cpp common/pptraclus.cpp)
target_link_libraries(server ABY::aby ENCRYPTO_utils::encrypto_utils)
target_link_libraries(client ABY::aby ENCRYPTO_utils::encrypto_utils)
