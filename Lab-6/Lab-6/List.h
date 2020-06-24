#pragma once
#include <exception>

template<typename T>
class List
{
private:
	struct Node;
public:
	class Iterator
	{
	public:
		Iterator( Node* node ) : m_node( node ) {};
		~Iterator() {};

		bool operator== ( const Iterator& other ) const {
			if ( this == &other )
			{
				return true;
			}
			return m_node == other.m_node;
		}

		bool operator!= ( const Iterator& other ) const {
			return !operator==( other );
		}

		T operator*() const {
			if ( m_node )
			{
				return m_node->m_value;
			}
			throw std::exception( "Out of range" );
		}

		void operator++() {
			if ( m_node )
			{
				m_node = m_node->m_next;
			}
			else
			{
				throw std::exception( "Out of range" );
			}
		}
	private:
		Node* m_node;
	};
public:
	List() : m_head( NULL ) {};
	~List() { Clear(); };
	void Add( const T& value ) {
		Node* node = new Node( value );
		node->m_next = m_head;
		m_head = node;
		m_size++;
	};
	T Head() const {
		return m_head->m_value;
	};
	size_t Size() {
		return m_size;
	};
	void Remove() {
		if ( m_head )
		{
			Node* newHead = m_head->m_next;
			delete m_head;
			m_head = newHead;
			m_size--;
		}
	};
	void Clear() {
		while ( m_head )
		{
			Remove();
		}
	};
	Iterator Begin() const { return Iterator( m_head ); };
	Iterator End() const { return Iterator( NULL ); };
private:
	struct Node
	{
		Node() : m_next( nullptr ) {};
		Node( const T& value ) : m_value( value ) {};
		T m_value;
		Node* m_next;
	};
	Node* m_head;
	size_t m_size;
};
