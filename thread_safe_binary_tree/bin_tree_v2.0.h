#pragma once

#include <mutex>

template <class T>
struct node
{
	T key;
	node<T>* left = NULL;
	node<T>* right = NULL;
	std::mutex m;
	node(T key):key(key){}
};

template <class T>
class bin_tree
{
	node<T>* root = NULL;
	std::mutex root_mutex;
	int elem_count = 0;
	std::mutex elem_count_change;
	void inc_elem_count();
	void dec_elem_count();
	void search_for_remove(T x, node<T>* & parent_pointer, node<T>* & current_pointer);
	void delete_node_without_children(node<T>* parent, node<T>* delete_node);
	void delete_node_with_one_child(node<T>* parent, node<T>* delete_node);
	void delete_node_with_both_children(node<T>* parent, node<T>* delete_node);
public:
	bin_tree(){}
	~bin_tree();
	node<T>* get_root(){return root;}
	int get_elem_count(){return elem_count;}
	void insert(T x);
	node<T>* search(T x);
	void remove(T x);
	void print(node<T>* p, int level);
};

template <class T>
void delete_tree(node<T>* x)
{
	if(x)
	{
		delete_tree(x->left);
		delete_tree(x->right);
		delete x;
	}
}

template <class T>
bin_tree<T>::~bin_tree()
{
	delete_tree(root);
}

template <class T>
void bin_tree<T>::inc_elem_count()
{
	elem_count_change.lock();
	elem_count++;
	elem_count_change.unlock();
}

template <class T>
void bin_tree<T>::dec_elem_count()
{
	elem_count_change.lock();
	elem_count--;
	elem_count_change.unlock();
}

template <class T>
void bin_tree<T>::insert(T x)
{
	node<T>* new_node = new node<T>(x);
	root_mutex.lock();
	if(!root)
	{
		root = new_node;
		inc_elem_count();
		root_mutex.unlock();
		return;
	}
	node<T>* current = root;
	current->m.lock();
	root_mutex.unlock();
	
	while(true)
	{
		if(new_node->key < current->key)
		{
			if(!current->left)
			{
				current->left = new_node;
				inc_elem_count();
				current->m.unlock();
				return;
			}
			else
			{
				current->left->m.lock();
				node<T>* save = current;
				current = current->left;
				save->m.unlock();
			}
		}
		else
		if(new_node->key == current->key)
		{
			current->m.unlock();
			delete new_node;
			return;
		}
		else
		{
			if(!current->right)
			{
				current->right = new_node;
				inc_elem_count();
				current->m.unlock();
				return;
			}
			else
			{
				current->right->m.lock();
				node<T>* save = current;
				current = current->right;
				save->m.unlock();
			}
		}
	}
}

template <class T>
node<T>* bin_tree<T>::search(T x)
{
	root_mutex.lock();
	if(!root)
	{
		root_mutex.unlock();
		return NULL;
	}
	node<T>* current = root;
	current->m.lock();
	root_mutex.unlock();
	while(current)
	{
		if(x == current->key)
		{
			current->m.unlock();
			return current;
		}
		node<T>* temp;
		if(x < current->key)
			temp = current->left;
		else
			temp = current->right;
		if(temp)
			temp->m.lock();
		node<T>* save = current;
		current = temp;
		save->m.unlock();
	}
	return NULL;
}

template <class T>
void bin_tree<T>::search_for_remove(T x, node<T>* & parent_pointer, node<T>* & current_pointer)
{
	root_mutex.lock();
	if(!root)
	{
		parent_pointer = NULL;
		current_pointer = NULL;
		root_mutex.unlock();
		return;
	}
	node<T>* current = root;
	current->m.lock();
	if(x == current->key)
	{
		parent_pointer = NULL;
		current_pointer = current;
		return;
	}
	root_mutex.unlock();
	node<T>* next;
	if(x < current->key)
		next = current->left;
	else
		next = current->right;
	if(next)
		next->m.lock();
	while(next)
	{
		if(x == next->key)
		{
			parent_pointer = current;
			current_pointer = next;
			return;
		}
		node<T>* temp;
		if(x < next->key)
			temp = next->left;
		else
			temp = next->right;
		if(temp)
			temp->m.lock();
		node<T>* save = current;
		current = next;
		next = temp;
		save->m.unlock();
	}
	current->m.unlock();
	parent_pointer = NULL;
	current_pointer = NULL;
	return;
}

template <class T>
void bin_tree<T>::delete_node_without_children(node<T>* parent, node<T>* delete_node)
{
	if(!parent)
	{
		root = NULL;
		delete_node->m.unlock();
		delete delete_node;
		root_mutex.unlock();
		dec_elem_count();
		return;
	}
	if(parent->left == delete_node)
		parent->left = NULL;
	else
		parent->right = NULL;
	parent->m.unlock();
	delete_node->m.unlock();
	dec_elem_count();
	delete delete_node;
}

template <class T>
void bin_tree<T>::delete_node_with_one_child(node<T>* parent, node<T>* delete_node)
{
	if(!parent)
	{
		if(!root->right)
			root = root->left;
		else
			root = root->right;
		root_mutex.unlock();
		delete_node->m.unlock();
		dec_elem_count();
		delete delete_node;
		return;
	}
	if(!delete_node->right)
	{
		if(parent->left == delete_node)
			parent->left = delete_node->left;
		else
			parent->right = delete_node->left;
	}
	else
	{
		if(parent->left == delete_node)
			parent->left = delete_node->right;
		else
			parent->right = delete_node->right;
	}
	parent->m.unlock();
	delete_node->m.unlock();
	dec_elem_count();
	delete delete_node;
}

template <class T>
void bin_tree<T>::delete_node_with_both_children(node<T>* parent, node<T>* delete_node)
{
	if(parent)
		parent->m.unlock();
	node<T>* min = delete_node->right;
	min->m.lock();
	if(!min->left)
	{
		delete_node->key = min->key;
		delete_node->right = min->right;
		if(!parent)
			root_mutex.unlock();
		min->m.unlock();
		delete min;
		delete_node->m.unlock();
		dec_elem_count();
		return;
	}
	min->left->m.lock();
	while(min->left->left)
	{
		node<T>* save = min;
		min->left->left->m.lock();
		min = min->left;
		save->m.unlock();
	}
	delete_node->key = min->left->key;
	if(!parent)
		root_mutex.unlock();
	if(!min->left->right)
		delete_node_without_children(min, min->left);
	else
		delete_node_with_one_child(min, min->left);
	delete_node->m.unlock();
}

template <class T>
void bin_tree<T>::remove(T x)
{
	node<T>* parent;
	node<T>* delete_node;
	search_for_remove(x, parent, delete_node);
	// if(parent)
	// 	parent->m.unlock();
	// if(delete_node)
	// 	delete_node->m.unlock();
	// if(!parent&&delete_node)
	// 	root_mutex.unlock();
	if(!delete_node)
		return;
	if(delete_node->left == delete_node->right)
	{
		// if(parent)
		// 	parent->m.unlock();
		// delete_node->m.unlock();
		// if(!parent)
		// 	root_mutex.unlock();
		delete_node_without_children(parent, delete_node);
	}
	else
	if(!delete_node->left || !delete_node->right)
	{
		if(parent)
			parent->m.unlock();
		delete_node->m.unlock();
		if(!parent)
			root_mutex.unlock();
		//delete_node_with_one_child(parent, delete_node);
	}
	else
	{
		if(parent)
			parent->m.unlock();
		delete_node->m.unlock();
		if(!parent)
			root_mutex.unlock();
		//delete_node_with_both_children(parent, delete_node);
	}
}

template <class T>
void bin_tree<T>::print(node<T>* p, int level)
{
    if(p)
    {
        print(p->right, level + 1);
        for(int i = 0; i < level; i++)
        	std::cout<<"   ";
        std::cout << p->key << std::endl;
        print(p->left, level + 1);
    }
}