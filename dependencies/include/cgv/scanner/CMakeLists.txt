project(cgv_scanner_dll)

#needed libs: alp.lib imaq.lib
add_library(cgv_scanner_dll SHARED highspeed_camera.cxx highspeed_projector.cxx)


