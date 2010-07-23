project(cgv_gui_fltk_lib)

add_library(cgv_gui_fltk_lib SHARED gui.cxx osg/MatrixTransformUI.cxx osg/Viewer.cxx osg/Window.cxx)
target_link_libraries(cgv_gui_fltk_lib cgv_registry_lib fltk2 fltk2_gl ${OSG_LINK_FLAGS})

add_library(cgv_gui_fltk_libS STATIC gui.cxx osg/MatrixTransformUI.cxx osg/Viewer.cxx osg/Window.cxx)
target_link_libraries(cgv_gui_fltk_libS cgv_registry_libS fltk2 fltk2_gl ${OSG_LINK_FLAGS})

#add_library(fltk2dll SHARED gui.cxx osg/MatrixTransformUI.cxx osg/Viewer.cxx osg/Window.cxx)
#target_link_libraries(fltk2dll cgv_registry_lib fltk2 fltk2_gl ${OSG_LINK_FLAGS})
