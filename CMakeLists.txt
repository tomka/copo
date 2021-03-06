cmake_minimum_required(VERSION 2.8)
project(copo)

## Compiler flags
if(CMAKE_COMPILER_IS_GNUCXX)
    list(APPEND CMAKE_CXX_FLAGS "-frounding-math") ## -frounding-math needed by CGAL
endif()

#subdirs(../dependencies/linux_include/cgv)
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	src/
	dependencies/fltk-2.0.x-r6786/
	dependencies/linux_include/
)

link_directories(
	../dependencies/fltk-2.0.x-r6786/lib
)

#add_definitions(-DAPIENTRY=/**/)

set(COPO_LIB
	-lboost_signals
	-lIL
	-lILU
	-lILUT
	-lGLEW
	-lXft
	-lXinerama
	-lCGAL
	-lfltk2
	-lfltk2_gl
	-lfltk2_glut
	-lfltk2_images
)

set(COPO_H
	src/Rendering/RenderingInfo.h
	src/Commands/Command.h
	src/Model/Flake_Schema_Small.h
	src/Model/Flake_Schema_DynStraight.h
	src/Model/EdgeGroup.h
	src/Model/World.h
	src/Model/Flake_Schema.h
	src/Model/Flake_Schema_Normal.h
	src/Model/ISky.h
	src/Model/IWind.h
	src/Model/EdgeGroupFace.h
	src/Model/Flake_Schema_Straight.h
	src/Model/SkyBucket.h
	src/Model/IGround.h
	src/Model/IAngleOfRepose.h
	src/Model/ILaunchSite.h
	src/Model/SnowModel.h
	src/Model/SkyGrid.h
	src/Model/AORDefault.h
	src/utilities.h
	src/Utilities/Color.h
	src/Utilities/Noise.h
	src/Utilities/glxext.h
	src/Utilities/OpenGL.h
	src/Utilities/glext.h
	src/Utilities/Streams.h
	src/Utilities/Events/events.h
	src/Utilities/wglext.h
	src/Utilities/Timer.h
	src/Utilities/Strings.h
	src/Utilities/math.h
	src/Utilities/MeshHelpers.h
	src/Utilities/glew-1.5.1-win32/glew/include/GL/wglew.h
	src/Utilities/glew-1.5.1-win32/glew/include/GL/glew.h
	src/Utilities/Random.h
	src/Utilities/Keys.h
	src/Utilities/PolygonOffset.h
	src/Utilities/node_wrap.h
	src/Utilities/Data.h
	src/Utilities/Path.h
	src/Utilities/Intersection/Intersection.h
	src/Utilities/Intersection/TriangleIntersection.h
	src/Mesh.h
	src/Geometry/LineSegment.h
	src/view.h
	src/MainControler.h
	src/IO/MeshReader.h
	src/IO/polygonal_mesh_obj_reader.h
	src/Interfaces/IIntersectable.h
	src/Shading/GLSL_Shader.h
	src/Shading/CG_Shader.h
	src/Shading/Texture.h
	src/Shading/Shader.h
	src/Shading/Material.h
	src/MeshRepresentation/CGALRepresentation/CGALConfig.h
	src/MeshRepresentation/CGALRepresentation/CGALAdapter.h
	src/MeshRepresentation/CGALRepresentation/CGALLaunchSite.h
	src/MeshRepresentation/MeshRepresentation.h
	src/MeshRepresentation/IMeshRepresentation.h
	src/MeshPool.h
	src/polygonal_mesh.h
	src/objects.h
	src/polygonal_mesh_drawable.h
	src/Exceptions/MeshPoolException.h
	src/Exceptions/SnowModelException.h
	src/Exceptions/NotInitializedException.h
	src/Exceptions/NotSupportedException.h
	src/Exceptions/NotImplementedException.h
	src/Exceptions/MeshLoaderException.h
	src/Exceptions/ArgumentException.h
	src/types.h
	src/Modifiers/ModifierAddObject.h
	src/Modifiers/ModifierSubdivisionApproximation.h
	src/Modifiers/Modifier.h
	src/Modifiers/ModifierShadeSnowModel.h
	src/Modifiers/ModifierCreateLaunchSites.h
	src/Modifiers/ModifierTriangulate.h
	src/Modifiers/ModifierCreateEdgeGroups.h
	src/Modifiers/SubDivide/subdiv_catmullclark.h
	src/Modifiers/SubDivide/subdiv_loop.h
	src/Modifiers/SubDivide/subdiv_scheme.h
	src/Modifiers/SubDivide/subdiv_face_cutting_quad.h
	src/Modifiers/SubDivide/subdiv_butterfly.h
	src/Modifiers/SubDivide/subdiv_kobbelt.h
	src/Modifiers/SubDivide/subdiv_face_cutting_triangle.h
	src/Modifiers/SubDivide/ModifierSubDivide.h
	src/Modifiers/Modifiers.h
	src/Modifiers/ModifierAccumulateSnow.h
	src/Modifiers/ModifierSnowStability.h
	src/Modifiers/Exceptions/PreConditionsViolatedException.h
	src/Modifiers/ModifierAddAdditionalBorderPoints.h
	src/Modifiers/ModifierRegularStorm.h
	src/GUI/NoiseWidget.h
	src/GUI/window.h
	src/GUI/AORWidget.h
)

set(COPO_CPP
	src/MainControler.cpp
	src/Rendering/RenderingInfo.cpp
	src/Model/EdgeGroupFace.cpp
	src/Model/SkyGrid.cpp
	src/Model/EdgeGroup.cpp
	src/Model/AORDefault.cpp
	src/Model/World.cpp
	src/Model/SnowModel.cpp
	src/Utilities/Random.cpp
	src/Utilities/PolygonOffset.cpp
	src/Utilities/OpenGL.cpp
	src/Utilities/Color.cpp
	src/Utilities/Timer.cpp
	src/Utilities/Events/events.cpp
	src/Utilities/MeshHelpers.cpp
	src/Utilities/Intersection/Intersection.cpp
	src/view.cpp
	src/utilities.cpp
	src/IO/MeshReader.cpp
	src/polygonal_mesh.cpp
	src/Shading/Shader.cpp
	src/Shading/Texture.cpp
	src/Shading/GLSL_Shader.cpp
	src/MeshRepresentation/CGALRepresentation/CGALLaunchSite.cpp
	src/MeshRepresentation/CGALRepresentation/CGALAdapter.cpp
	src/objects.cpp
	src/Mesh.cpp
	src/Modifiers/ModifierAddAdditionalBorderPoints.cpp
	src/Modifiers/ModifierRegularStorm.cpp
	src/Modifiers/ModifierAddObject.cpp
	src/Modifiers/ModifierTriangulate.cpp
	src/Modifiers/ModifierSnowStability.cpp
	src/Modifiers/ModifierCreateLaunchSites.cpp
	src/Modifiers/SubDivide/ModifierSubDivide.cpp
	src/Modifiers/ModifierCreateEdgeGroups.cpp
	src/Modifiers/ModifierSubdivisionApproximation.cpp
	src/Modifiers/ModifierAccumulateSnow.cpp
	src/Modifiers/ModifierShadeSnowModel.cpp
	src/GUI/window.cpp
	src/MeshPool.cpp
	src/snowSim.cxx
)

add_executable(../bin/copo
	${COPO_CPP}
	${COPO_H}
)

target_link_libraries(../bin/copo ${COPO_LIB})

