namespace cgv {
	namespace gui {
		namespace fltk {


struct element_layout
{
	int width,height,left_border,right_border,bottom_border,top_border;
	bool vert;
	int n, border_size;
	friend std::ostream& operator << (std::ostream& os, const element_layout& el)
	{
		return os << el.width << "x" << el.height << " - " 
			<< el.left_border << "|" << el.right_border
			<< ", " << el.bottom_border << "-" << el.top_border;
	}
	element_layout(bool _vert, int _n, int _bs) : vert(_vert), n(_n), border_size(_bs)
	{
		width=height=left_border=right_border=bottom_border=top_border=0;
	}
	element_layout(element_ptr e, ::fltk::Widget* w, bool _vert, int _n, int _bs) : vert(_vert), n(_n), border_size(_bs)
	{
		width  = w->w();
		height = w->h();
		left_border=right_border=bottom_border=top_border=0;
		label_ptr l(dynamic_cast<label*>(e.ref()));
		group*         g = dynamic_cast<group*>(l.ref());
		button*        b = dynamic_cast<button*>(l.ref());
		abst_observer* o = dynamic_cast<abst_observer*>(l.ref());
		if (!l.empty() && (g || b || o) && l->get_placement() != label::NO_LABEL) {
			int wi, he;
			w->measure_label(wi,he);
			switch (l->get_placement()) {
			case label::LEFT_LABEL   : 
				left_border = wi; 
				break;
			case label::RIGHT_LABEL  : 
				right_border = wi; 
				break;
			case label::BOTTOM_LABEL : 
				bottom_border = he;
				break;
			case label::TOP_LABEL    : 
				top_border = he;
				break;
			}
			if (vert) {
				if (l->get_placement() == label::BOTTOM_LABEL ||
					l->get_placement() == label::TOP_LABEL)
						if (wi > width)
							right_border = left_border = (wi-width+1)/2;
			}
			else {
				if (l->get_placement() == label::LEFT_LABEL ||
					l->get_placement() == label::RIGHT_LABEL)
						if (he > height)
							top_border = bottom_border = (he-height+1)/2;
			}
		}
	}
	void concat(const element_layout& el)
	{
		if (vert) {
			if (el.width > width)
				width = el.width;
			if (el.left_border > left_border)
				left_border = el.left_border;
			if (el.right_border > right_border)
				right_border = el.right_border;
			height += bottom_border+el.height+el.top_border;
			bottom_border = el.bottom_border;
		}
		else {
			if (el.height > height)
				height = el.height;
			if (el.bottom_border > bottom_border)
				bottom_border = el.bottom_border;
			if (el.top_border > top_border)
				top_border = el.top_border;
			width += el.width+el.left_border+right_border;
			right_border = el.right_border;
		}
	}
	int total_width() const
	{
		if (vert)
			return width + 2*border_size+left_border+right_border;
		else
			return width + border_size*(n+1)+left_border+right_border;
	}
	int total_height() const
	{
		if (vert)
			return height + border_size*(n+1)+top_border+bottom_border;
		else
			return height + 2*border_size+top_border+bottom_border;
	}
};

void layout_group(group_ptr g, ::fltk::Group* fg, bool propagate_upward)
{
	const int border_size = 5;

	bool vert = g->get_options() & group::VERTICAL;
	unsigned int i;
	unsigned int n = g->get_nr_children();
	element_layout group_layout(vert, n, border_size);
	for (i=0; i<n; ++i) {
		group_layout.concat(element_layout(g->get_child(i), fg->child(i), vert, n, border_size));
	}
	std::cout << "group " << g->get_name().c_str() << " layout: " << group_layout << std::endl;
	bool enlarged = group_layout.total_width() > fg->w() || group_layout.total_height() > fg->h();
	if (enlarged)
		fg->resize(group_layout.total_width(), group_layout.total_height());

	int x = border_size+(vert?group_layout.left_border:0);
	int y = border_size+(vert?0:group_layout.top_border);
	for (i=0; i<n; ++i) {
		::fltk::Widget* w = fg->child(i);
		element_layout el(g->get_child(i), fg->child(i), vert, n, border_size);
		if (vert) {
			y += el.top_border;
			w->position(x,y);
			y += border_size+el.height+el.bottom_border;
		}
		else {
			x += el.left_border;
			w->position(x,y);
			x += border_size+el.width+el.right_border;
		}
	}
	fg->init_sizes();
/*	std::cout << "group layout: " << *fg << std::endl;
	for (i=0; i<n; ++i) {
		std::cout << "   child " << i << " : " << *fg->child(i) << std::endl;
		group* cg = dynamic_cast<group*>(g->get_child(i).ref());
		if (cg) {
			::fltk::Group* fh = (::fltk::Group*) fg->child(i);
			for (int j=0; j<fh->children(); ++j)
				std::cout << "      child " << j << " : " << *fh->child(j) << std::endl;
		}
	}
*/
	if (enlarged && propagate_upward) {
		group_ptr p;
		::fltk::Group* fp = 0;
		put_parent_group(g, fg, p, fp);
		if (fp)
			layout_group(p, fp, true);
	}
}

		}
	}
}