#ifndef H_LINKED_LIST
#define H_LINKED_LIST

#include <stdint.h>
#include <stddef.h>
#include "mem.hpp"

template<typename T>
class ListNode
{
public:
    ListNode<T>* next = NULL;
    T val;
    T* Add(T x);
    T* GetAt(int i);
    void Free();
};

template<typename T>
class LinkedList
{
private:
    ListNode<T>* begin;
    ListNode<T>* end;
public:
    size_t size;
    LinkedList();
    void PushBack(T val);
    void PopBack();
    T& operator[](int I);
};

template<typename T>
T* ListNode<T>::Add(T x)
{
    if (next)
    {
        ListNode<T>* oldNext = next;
        next = (ListNode<T>*)kmalloc(sizeof(ListNode<T>));
        next->val = x;
        next->next = oldNext;
        return next;
    }

    next = (ListNode<T>*)kmalloc(sizeof(ListNode<T>));
    next->val = x;
    next->next = NULL;
    return next;
}
template<typename T>
T* ListNode<T>::GetAt(int i)
{
    ListNode<T>* curNode = this;
    while (i--)
    {
        if (!curNode->next) return NULL;
        curNode = curNode->next;
    }
    return curNode;
}
template<typename T>
void ListNode<T>::Free()
{
    if (next) 
    {
        next->Free();
        next = NULL;
    }
    kfree(this);
}

template<typename T>
LinkedList<T>::LinkedList()
{
    begin = (ListNode<T>*)kmalloc(sizeof(ListNode<T>));
    end = begin;
    size = 0;
}
template<typename T>
void LinkedList<T>::PushBack(T val)
{
    if (size == 0)
    {
        size++;
        begin->val = val;
        begin->next = (ListNode<T>*)kmalloc(sizeof(ListNode<T>));
        end = begin->next;
        return;
    }
    size++;
    end->val = val;
    end->next = (ListNode<T>*)kmalloc(sizeof(ListNode<T>));
    end = end->next;
}
template<typename T>
void LinkedList<T>::PopBack()
{
    if (size == 0)
    {
        return;
    }
    size--;
    // This, this can be optimized.
    ListNode<T>* cur = begin;
    int steps = size;
    while (steps--) cur = cur->next;
    kfree(cur->next);
    cur->next = NULL;
}
template<typename T>
T& LinkedList<T>::operator[](int I)
{
    // This, this too can be optimized.
    if (I == 0) return begin->val;
    ListNode<T>* cur = begin;
    int steps = I - 1;
    while (steps-- > 0) cur = cur->next;
    return cur->next->val;
}

#endif // H_LINKED_LIST