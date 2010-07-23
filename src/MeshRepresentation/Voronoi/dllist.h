/*****************************************************************************
**
** dllist.h: an include file for (doubly-)linked lists.
**
** Copyright (C) 1995 by Dani Lischinski
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
******************************************************************************/

#ifndef LLIST_H
#define LLIST_H

typedef void* Ldata;

class Llist;

class LlistNode {
	friend class Llist;
private:
	Ldata data;
	LlistNode *next, *prev;
	LlistNode()                         { data = 0; next = prev = this; }
	LlistNode(Ldata d, LlistNode* n)    { data = d; next = n;
	                                      prev = n->prev; n->prev = this; }
};

typedef LlistNode *LlistPos;

class Llist {
private:
	int len;
	LlistNode *node;
public:
	Llist()                             { node = new LlistNode; len = 0; }
	Boolean empty() const               { return (node == node->next); }
	Boolean isEnd(LlistPos p) const     { return (p->next == node); }
	LlistPos first() const              { return node; }
	LlistPos next(LlistPos p) const     { return p->next; }
	LlistPos prev(LlistPos p) const     { return p->prev; }
	Ldata retrieve(LlistPos p) const    { return p->next->data; }
	void store(LlistPos p, Ldata d)     { p->next->data = d; }
	int length() const                  { return len; }
	LlistNode* insert(LlistPos, Ldata);
	void remove(LlistPos);
	~Llist();
};

inline LlistNode* Llist::insert(LlistPos p, Ldata d)
{
	len++;
	return p->next = new LlistNode(d, p->next);
}

inline void Llist::remove(LlistPos p)
{
	LlistNode *t = p->next;
	p->next = t->next;
	t->next->prev = t->prev;
	delete t;
	len--;
}

inline Llist::~Llist()
{
	while (!empty()) remove(first());
	delete node;
}

#endif

