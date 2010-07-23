#include <iostream>
#include <algorithm>
#include <cgv/gui/fltk/osg/MatrixTransformUI.h>
#include <cgv/gui/fltk/osg/fltk_node_ui_factory.h>

using namespace fltk;

namespace cgv {
	namespace gui {
		namespace fltk {
			namespace osg {

// register the ui
static register_fltk_node_ui<MatrixTransformUI> MTUI("MatrixTransform", "SimpleMatrixUI");

using namespace ::fltk;

void MatrixTransformUI::value_changed_cb(::fltk::FloatInput* floatInput)
{
	// check if node is defined
	if (!matrix_transform)
		return;
	// find the index i of the changed input
	FloatInput** changedInput = std::find(floatInputs,floatInputs+16,floatInput);
	if (changedInput == floatInputs+16)
		return;
	int i = changedInput - floatInputs;
	// update the i-th entry of the matrix
	::osg::Matrix matrix = matrix_transform->getMatrix();
	matrix.ptr()[i] = floatInput->fvalue();
	matrix_transform->setMatrix(matrix);
	// cause a value changed callback
	do_callback();
}

/// construct 
MatrixTransformUI::MatrixTransformUI(int x, int y, int w, int h, const char* label) : Group(x,y,w,h,label)
{
	// init node pointer
	matrix_transform = 0;
	// compute available extents
	float fieldWidth = w*0.2f;
	float borderWidth = fieldWidth*0.2f;
	float fieldHeight = h*0.2f;
	if (fieldHeight > 20)
		fieldHeight = 20;
	float borderHeight = fieldHeight*0.2;
	// construct fltk FloatInputs
	int i = 0;
	begin();
	for (int xi=0; xi<4; ++xi) {
		for (int yi=0; yi<4; ++yi,++i) {
			floatInputs[i] = new FloatInput((int)(xi*(borderWidth+fieldWidth)+borderWidth),
				                            (int)(yi*(borderHeight+fieldHeight)+borderHeight),
											(int) fieldWidth, (int) fieldHeight);
			floatInputs[i]->callback(value_changed_cb, this);
		}
	}
	// pop the group widget from the Widget stack of fltk
	end();
}

/// integrate into tab group, don't forget to set the resizable of the tab group
void MatrixTransformUI::integrate(::fltk::TabGroup& tab_group)
{
	tab_group.add(this);
	tab_group.resizable(this);
}

/// attach the node and increment reference count of node
void MatrixTransformUI::node(::osg::Node* node)
{
	// convert node to MatrixTransform
	::osg::Transform* transform = node->asTransform();
	if (!transform) {
		std::cerr << "attempt to attach a node of type " << node->className() << " to a MatrixTransformUI" << std::endl;
		return;
	}
	::osg::MatrixTransform* new_matrix_transform = transform->asMatrixTransform();
	if (!new_matrix_transform) {
		std::cerr << "attempt to attach a transform node of type " << node->className() << " to a MatrixTransformUI" << std::endl;
		return;
	}
	// set member and reference node
	if (matrix_transform) 
		matrix_transform->unref();
	matrix_transform = new_matrix_transform;
	matrix_transform->ref();
	// update the gui
	refresh_gui();
}

/// destructor decrements reference count to node
MatrixTransformUI::~MatrixTransformUI()
{
	if (matrix_transform)
		matrix_transform->unref();
}

void MatrixTransformUI::refresh_gui()
{
	if (!matrix_transform)
		return;
	for (int i=0; i<16; ++i)
		floatInputs[i]->value(matrix_transform->getMatrix().ptr()[i]);
}

/// return a pointer to the attached node
::osg::Node* MatrixTransformUI::node() const
{
	return matrix_transform;
}
			}
		}
	}
}
