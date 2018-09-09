#pragma once

#include <mutex>

std::mutex write_mutex;

template <class T>
class node
{
public:
	T key;
	node* l = NULL;
	node* r = NULL;
	node* parent = NULL;
	std::mutex m;
	node(T key):key(key){}
	T get_key(){return key;}
	void set_key(T new_key)
	{
		key = new_key;
	}
};

template <class T>
class bin_tree
{
public:
	node<T>* root;
	int elem_count = 0;
	std::mutex elem_count_change;
	std::mutex root_mutex;

	bin_tree():root(NULL){}
	~bin_tree();
	void insert(T x);
	node<T>* search(T x);
	void remove(T x);
	node<T>* find_min(node<T>* x);
	void show();
	int get_elem_count(){return elem_count;}
	node<T>* get_root(){return root;}
	void delete_root_without_children();
	void delete_root_with_right_child_only();
	void delete_root_with_left_child_only();
	void delete_node_with_both_children(node<T>* x, bool is_root);
	void delete_node_without_children(node<T>* x);
	void delete_node_with_right_child_only(node<T>* x);
	void delete_node_with_left_child_only(node<T>* x);
	void print_Tree(node<T>* p, int level);
};

template <class T>
void delete_tree(node<T>* x)
{
	if(x)
	{
		delete_tree(x->l);
		delete_tree(x->r);
		delete x;
	}
}

template <class T>
bin_tree<T>::~bin_tree()
{
	delete_tree(root);
}

template <class T>
void bin_tree<T>::insert(T x)
{
	node<T>* new_node = new node<T>(x);
	root_mutex.lock();//чтобы только 1 поток изменил root
	node<T>* current = root;
	node<T>* parent = NULL;
	if(current)
	{
		current->m.lock();
		root_mutex.unlock();
	}
	while(current != NULL)
	{
		if(parent)
			parent->m.unlock();
		parent = current;
		if(new_node->get_key() < current->get_key())
		{
			current = current->l;
			if(current)
				current->m.lock();
		}
		else
		{
			current = current->r;
			if(current)
				current->m.lock();
		}
	}
	new_node->parent = parent;
	if(!parent)
	{
		root = new_node;
		root_mutex.unlock();
	}
	else
	{
		if(new_node->get_key() < parent->get_key())
			parent->l = new_node;
		else
			parent->r = new_node;
		parent->m.unlock();
	}
	elem_count_change.lock();
	elem_count++;
	elem_count_change.unlock();
}

template <class T>
node<T>* bin_tree<T>::search(T x)
{
	root_mutex.lock();
	node<T>* current = root;
	root_mutex.unlock();
	if(current)
		current->m.lock();
	while(true)
	{
		if(!current)
			return NULL;
		if(x == current->get_key())
		{
			current->m.unlock();
			return current;
		}
		if(x < current->get_key())
		{
			if(current->l)
				current->l->m.lock();
			node<T>* save = current;
			current = current->l;
			save->m.unlock();
		}
		else
		if(x > current->get_key())
		{
			if(current->r)
				current->r->m.lock();
			node<T>* save = current;
			current = current->r;
			save->m.unlock();
		}
	}
}

template <class T>
void bin_tree<T>::delete_root_without_children()
{
	root->m.unlock();
	delete root;
	root = NULL;
	elem_count_change.lock();
	elem_count--;
	elem_count_change.unlock();
}

template <class T>
void bin_tree<T>::delete_root_with_left_child_only()
{
	root->l->m.lock();
	node<T>* save = root->l;
	root->m.unlock();
	delete root;
	root = save;
	root->parent = NULL;//
	save->m.unlock();
	elem_count_change.lock();
	elem_count--;
	elem_count_change.unlock();
}

template <class T>
void bin_tree<T>::delete_root_with_right_child_only()
{
	root->r->m.lock();
	node<T>* save = root->r;
	root->m.unlock();
	delete root;
	root = save;
	root->parent = NULL;//
	save->m.unlock();
	elem_count_change.lock();
	elem_count--;
	elem_count_change.unlock();
}

template <class T>
void bin_tree<T>::delete_node_with_both_children(node<T>* x, bool is_root)
{
	node<T>* min = x->r;
	min->m.lock();
	if(!min->l)
	{
		x->l->m.lock();
		min->l = x->l;
		x->l->parent = min;
		x->l->m.unlock();
		if(is_root)//(x == root)
		{
			root = min;
			root->parent = NULL;
			x->m.unlock();
			delete x;
			min->m.unlock();
			elem_count_change.lock();
			elem_count--;
			elem_count_change.unlock();
			return;
		}
		if(x == x->parent->l)
			x->parent->l = min;
		else
			x->parent->r = min;
		min->parent = x->parent;
		x->parent->m.unlock();
		min->m.unlock();
		x->m.unlock();
		delete x;
		elem_count_change.lock();
		elem_count--;
		elem_count_change.unlock();
		return;
	}
	if(x->parent)
		x->parent->m.unlock();
	min->l->m.lock();
	while(min->l->l)
	{
		node<T>* save = min;
		min->l->l->m.lock();
		min = min->l;
		save->m.unlock();
	}
	x->key = min->l->key;
	if(!min->l->r)
	{
		delete_node_without_children(min->l);
		x->m.unlock();
		return;
	}
	delete_node_with_right_child_only(min->l);
	x->m.unlock();
}

template <class T>
void bin_tree<T>::delete_node_without_children(node<T>* x)
{
	node<T>* parent = x->parent;
	if(parent->l == x)
	{
		parent->l = NULL;
	}
	else
	{
		parent->r = NULL;
	}
	x->m.unlock();
	delete x;
	parent->m.unlock();
	elem_count_change.lock();
	elem_count--;
	elem_count_change.unlock();
}

template <class T>
void bin_tree<T>::delete_node_with_left_child_only(node<T>* x)
{
	node<T>* parent = x->parent;
	node<T>* left = x->l;
	left->m.lock();
	if(parent->l == x)
	{
		parent->l = left;
	}
	else
	{
		parent->r = left;
	}
	left->parent = parent;
	x->m.unlock();
	delete x;
	left->m.unlock();
	parent->m.unlock();
	elem_count_change.lock();
	elem_count--;
	elem_count_change.unlock();
}

template <class T>
void bin_tree<T>::delete_node_with_right_child_only(node<T>* x)
{
	node<T>* parent = x->parent;
	node<T>* right = x->r;
	right->m.lock();
	if(parent->l == x)
	{
		parent->l = right;
	}
	else
	{
		parent->r = right;
	}
	right->parent = parent;
	x->m.unlock();
	delete x;
	right->m.unlock();
	parent->m.unlock();
	elem_count_change.lock();
	elem_count--;
	elem_count_change.unlock();
}


template <class T>
void bin_tree<T>::remove(T x)
{
	root_mutex.lock();
	if(!root)
	{
		root_mutex.unlock();
		return;
	}
	root->m.lock();
	if(x == root->get_key())//удаляем корень
	{
		if(root->l == root->r)
		{
			delete_root_without_children();
			root_mutex.unlock();
			return;
		}
		if(root->l && !root->r)
		{
			delete_root_with_left_child_only();
			root_mutex.unlock();
			return;
		}
		if(!root->l && root->r)
		{
			delete_root_with_right_child_only();
			root_mutex.unlock();
			return;
		}
		if(root->l && root->r)
		{
			delete_node_with_both_children(root, true);
			root_mutex.unlock();
			return;
		}
	}
	
	node<T>* current = root;
	root_mutex.unlock();
	node<T>* next;
	if(x < current->key)
		next = current->l;
	else
		next = current->r;
	if(next)
	{
		next->m.lock();
	}
	else
	{
		root->m.unlock();
		return;
	}
	while(next && next->key != x)
	{
		node<T>* save = next;
		if(x < next->key)
		{
			next = next->l;
		}
		else
		{
			next = next->r;
		}
		if(next)
		{
			next->m.lock();
		}
		current->m.unlock();
		current = save;
	}
	if(!next)
	{
		current->m.unlock();
		return;
	}
	if(next->l == next->r)
	{
		delete_node_without_children(next);
	}
	else
	if(next->l && !next->r)
	{
		delete_node_with_left_child_only(next);
	}
	else
	if(!next->l && next->r)
	{
		delete_node_with_right_child_only(next);
	}
	else
	{
		delete_node_with_both_children(next, false);
	}
}

template <class T>
node<T>* bin_tree<T>::find_min(node<T>* elem)
{
	node<T>* x = elem;
	x->m.lock();
	while(x->l)
	{
		x->l->m.lock();
		node<T>* save = x;
		x = x->l;
		save->m.unlock();
	}
	x->m.unlock();
	return x;
}

template <class T>
void bin_tree<T>::show()
{
	show_tree(root);
}

template <class T>
void show_tree(node<T>* x)
{
	if(x)
	{
		show_tree(x->l);
		std::cout << x->get_key() << std::endl;
		show_tree(x->r);
	}
}

template <class T>
void who_is_parent(node<T>* x)
{
	if(x)
	{
		who_is_parent(x->l);
		std::cout << "x: " << x->get_key();
		if(x->parent)
			std::cout << "->" << x->parent->get_key() << std::endl;
		else
			std::cout << "->NULL" << std::endl;
		who_is_parent(x->r);
	}
}

template <class T>
void bin_tree<T>::print_Tree(node<T>* p, int level)
{
    if(p)
    {
        print_Tree(p->r, level + 1);
        for(int i = 0; i < level; i++)
        	std::cout<<"   ";
        std::cout << p->key << std::endl;
        print_Tree(p->l, level + 1);
    }
}