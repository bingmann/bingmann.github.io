/*
 * $Id$
 * 
 * An insert only red-back tree implementation
 * @author matze
 */
#ifndef REDBACKTREE_H_
#define REDBACKTREE_H_

#include <assert.h>
#include <stdlib.h>

#define DEBUG_VERIFY

#ifdef DEBUG_VERIFY
#include <stdio.h>
#endif

//#define TRACE(x, ...)	printf(x, __VA_ARGS__);
#define TRACE(x, ...)

namespace std {

	namespace private_stuff
	{
		template<typename KeyType, typename ValueType>
		class RedBlackTree
		{
		private:
			class Node
			{
			public:
			    enum Color { RED, BLACK };
			    
			    ValueType data;
			    Node* parent;
			    Node* left;
			    Node* right;
			    Color color;
			    
			    Node(const ValueType& newdata)
			    	: data(newdata), left(NULL), right(NULL)
			    {
			    }
			};
		
		public:
			class iterator
			{
				Node* node;
				
			public:
				explicit iterator(Node* newnode)
					: node(newnode)
				{}
				
				ValueType& operator*()
				{
					return node->data;
				}
			
				ValueType* operator->()
				{
					return &node->data;
				}
				
				bool operator ==(const iterator& other) const
				{
					return node == other.node;
				}

				bool operator !=(const iterator& other) const
				{
					return node != other.node;
				}
				
				iterator& operator++()
				{
					if(node->right != NULL) {
						node = node->right;
						while(node->left != NULL)
							node = node->left;
						return *this;
					}
					
					if(node->parent == NULL) {
						node = NULL;
						return *this;
					}
					
					while(node->parent != NULL) {
						if(node->parent->left == node) {
							node = node->parent;
							return *this;
						}
						node = node->parent;
					}
					
					node = NULL;
					return *this;			
				}
			};
		
		    RedBlackTree()
		    {
		        root = NULL;
		        nodecount = 0;
		    }
		    
		    ~RedBlackTree()
		    {
		    	clear();
		    }
		
		    iterator find(const KeyType& key)
		    {
				Node* current = root;
				while(current != NULL) {
					if(current->data == key)
						return iterator(current);
				
					if(current->data < key)
						current = current->right;
					else
						current = current->left;
				}
				
				return end();
		    }
		    
		    iterator begin()
		    {
		    	if(root == NULL)
		    		return end();
		    	
				Node* node = root;  	
		    	while(node->left != NULL) {
		    		node = node->left;
		    	}
		    	return iterator(node);
		    }
		    
		    iterator end()
		    {
		    	return iterator(NULL);
		    }
		
		    iterator insert(const KeyType& key, const ValueType& value)
		    {
				if(root == NULL) {
					Node* node = new Node(value);
					node->color = Node::BLACK;
					node->parent = NULL;
					root = node;
					TRACE("NewNode: %p\n", node);
					nodecount = 1;
					return iterator(node);
				}
				
				Node* node;
		 		Node* current = root;
				while(current != NULL) {
					if(current->data == key) {
						current->data = value;
						return iterator(current);
					}
		
					if(current->data < key) {
						if(current->right == NULL) {
							node = new Node(value);
							TRACE("NewNode: %p\n", node);
							current->right = node;
							TRACE("Inserting right of %p\n", current);
							break;
						} else {
							current = current->right;
						}						
					} else {
						if(current->left == NULL) {
							node = new Node(value);
							TRACE("NewNode: %p\n", node);
							current->left = node;
							TRACE("Inserting left of %p\n", current);
							break;
						} else {
							current = current->left;
						}
					}
				}

				node->color = Node::RED;				
				node->parent = current;
				balance(node);
				nodecount++;
				return iterator(node);
			}
			
			size_t size() const
			{
				return nodecount;
			}
			
			void clear()
			{
				clear(root);
				root = NULL;
			}
		
		private:
			void clear(Node* node)
			{
				if(node == NULL)	
					return;
					
				if(node->left) {
					clear(node->left);
				}
				if(node->right) {
					clear(node->right);
				}
				delete node;
			}
		
			void balance(Node* node)
			{
				TRACE("Balancing node %p\n", node);
				assert(node->color == Node::RED);
		
				Node* parent = node->parent;
				if(parent == NULL) {
					node->color = Node::BLACK;
#ifdef DEBUG_VERIFY
					verify_node(node, node->parent);
#endif
					return;
				}
		
				if(parent->color == Node::BLACK) {
#ifdef DEBUG_VERIFY
					verify_node(parent, parent->parent);
#endif
					return;
				}
		
				bool parent_at_left;
				Node* grandparent = parent->parent;
				assert(grandparent != NULL);
				Node* uncle;
				if(parent == grandparent->left) {
					uncle = grandparent->right;
					parent_at_left = true;
				} else {
					assert(parent == grandparent->right);
					uncle = grandparent->left;
					parent_at_left = false;
				}
		
				if(uncle != NULL && uncle->color == Node::RED) {
					uncle->color = Node::BLACK;
					parent->color = Node::BLACK;
					grandparent->color = Node::RED;
					balance(grandparent);
					return;
				}
		
				bool node_at_left;
				if(node == parent->left) {
					node_at_left = true;
				} else {
					assert(node == parent->right);
					node_at_left = false;
				}
		
				if(parent_at_left) {
					TRACE("%s of %p\n", node_at_left ? "right rot" : "leftright rot", grandparent);
					if(!node_at_left) {
						rotate_left(parent);
						node->color = Node::BLACK;
					} else {
						parent->color = Node::BLACK;
					}
					grandparent->color = Node::RED;
					rotate_right(grandparent);
#ifdef DEBUG_VERIFY
					verify_node(root, NULL);
#endif
				} else if(!parent_at_left) {
					TRACE("%s of %p\n", node_at_left ? "rightleft rot" : "left rot",
							grandparent);
					if(node_at_left) {
						rotate_right(parent);
						node->color = Node::BLACK;
					} else {
						parent->color = Node::BLACK;
					}
					grandparent->color = Node::RED;
					rotate_left(grandparent);
#ifdef DEBUG_VERIFY
					verify_node(root, NULL);
#endif
				}
		    }
		
			void rotate_right(Node* node)
			{	
				Node* oldparent = node->parent;
		
				assert(node->left != NULL);	
				Node* leftright = node->left->right;
				
				Node* newroot = node->left;
				newroot->right = node;
				newroot->parent = oldparent;
				node->left = leftright;
				node->parent = newroot;
				if(leftright != NULL)
					leftright->parent = node;
		
				if(oldparent == NULL) {
					root = newroot;
				} else {
					if(oldparent->left == node) {
						oldparent->left = newroot;
					} else {
						assert(oldparent->right == node);
						oldparent->right = newroot;
					}
				}
			}
		
			void rotate_left(Node* node)
			{	
				Node* oldparent = node->parent;
		
				assert(node->right != NULL);	
				Node* rightleft = node->right->left;
				
				Node* newroot = node->right;
				newroot->left = node;
				newroot->parent = oldparent;
				node->right = rightleft;
				node->parent = newroot;
				if(rightleft != NULL)
					rightleft->parent = node;
		
				if(oldparent == NULL) {
					root = newroot;
				} else {
					if(oldparent->left == node) {
						oldparent->left = newroot;
					} else {
						assert(oldparent->right == node);
						oldparent->right = newroot;
					}
				}
			}
		
#ifdef DEBUG_VERIFY
			int verify_node(const Node* node, const Node* parent)
			{
				//printf("Node %p Left: %p Right: %p Color: %s\n", node, node->left, node->right, node->color == Node::BLACK ? "black" : "red");
				if(node->parent != parent) {
					printf("Node %p: parent is %p but should be %p\n", node, node->parent, parent);
				}
				const Node* left = node->left;
				const Node* right = node->right;
				if(left != NULL && !(left->data < node->data)) {
					printf("Node %p: Left data is not smaller than data!\n", node);
					printf("Aborting\n");
					return 0;
				}
				if(right != NULL && right->data < node->data) {
					printf("Node %p: Right data is smaller than data!\n", node);
					printf("Aborting\n");
					return 0;
				}
		
				if(parent == NULL && node->color != Node::BLACK) {
					printf("Rootnode %p is not black!\n", node);
				}
				if(node->color == Node::RED) {
					if(left != NULL && left->color != Node::BLACK) {
						printf("Node %p is red but left %p is not black\n", node, left);
					}
					if(right != NULL && right->color != Node::BLACK) {
						printf("Node %p is red but right %p is not black\n",
								node, right);
					}
				}
		
				if(left == NULL && right == NULL) {
					if(node->color == Node::BLACK)
						return 2;
					else
						return 1;
				}
				int blacknodesleft = 1;
				int blacknodesright = 1;
				if(left != NULL) {
					blacknodesleft = verify_node(node->left, node);
				}
				if(right != NULL) {
					blacknodesright = verify_node(node->right, node);
				}
				if(blacknodesleft != blacknodesright) {
					printf("Node %p: blacknodecount left (%d) != blacknodecount right (%d)\n",
							node, blacknodesleft, blacknodesright);
				}
		
				if(node->color == Node::BLACK)
					return blacknodesleft + 1;
				else
					return blacknodesleft;
			}
#endif
			Node* root;
			size_t nodecount;
		};		
	}
}

#endif
