#ifndef TQ_H
#define TQ_H
#include <vector>
#include <iostream>
#include "Request.h"
using namespace std;
/**
Basic Idea:
We need O(1) access to elements, as well as a system that can push and pop in
FIFO fashion. We shoud also be able to delete certain element, without 
affecting order of other elements.
This motivates to use a double linked list of requests with some ID,
along with a positions vector which stores pointer to the request.
**/


// Node containing request, and previous and next elements of list
struct Node{
  Request req;
  Node* next;
  Node* prev;
};

// Doubly Linked List Implementation
class MyList {
 public:
 
  // constructor function
  MyList(){
    head=0;
    tail=0;
  }

  // push function, adds element at end
  // updates head and tail pointer
  Node* push_back(Request c){
    // if list is not empty
    if (head!=0){
      tail->next=new Node;
      tail->next->req=c;
      tail->next->next=0;
      tail->next->prev=tail;
      tail=tail->next;
    }
    // if list is empty
    else {
      head=new Node;
      head->req=c;
      head->next=0;
      head->prev=0;
      tail=head;
    }
    // returns pointer to element added
    return tail;
  } 
      
  // returns pointer to first node in list     
  Node* front(){
    return head;
  } 
  
  // pop function
  // removes first node and returns pointer to node removed
  Node* removefront(){
    //if list has more than 1 elements
    if (head!=tail){
      Node* k=head;
      head=head->next;
      head->prev=0;
      return k;
    }
    // if list has only one element
    else {
      Node* k=head;
      head=0;
      tail=0;
      //return 0;
    }
  }      
  
  // delete node with given pointer
  void deleteNode(Node *ptr){
    // if the given pointer is head
    if (ptr==head) {
      removefront();
    }
    // if given pointer is tail
    // remove tail and set new tail
    else if (ptr==tail){
      Node *k =tail;
      tail=tail->prev;
      tail->next=0;
    }
    // in other cases, remove the given element and 
    // update prev and next of nearby elements
    else {  
      Node *k=ptr->next;
      Node *j=ptr->prev;
      j->next=k;
      k->prev=j;
      }   
  } 
   
 // private members   
 private:
  Node *head;	
  Node *tail;	
};

// TimeOutQueue Implementation 
class TimeOutQueue{
  // doubly linked list with requests
  MyList array;
  // positions array containing pointer to requests
  Node** positions;
  // size of queue 
  int size1;
 public:
 
  // constructor function
  TimeOutQueue(){
    size1=0;
  }
   
  // set size of positions array 
  void setnmax(int k){
    positions=new Node*[k];
  }
  
  // return size of queue
  int size(){ 
    return size1;
  }
  
  // push function
  // add a new request to the back of queue
  // update system variables
  void push(Request& x){
    Node* n=array.push_back(x);
    size1++;
    positions[x.getID1()]=n;
   }

  // pop function
  // remove front request from array, and returns the request
  Request pop(){
    Node* k=array.removefront();
    size1--;
    positions[(k->req).getID1()]=0;
    return (k->req);
   }
   
  // returns front request 
  Request front(){
    Node* k=array.front();
    return k->req;
  }
  
  // check if queue is empty
  bool empty(){
    return (size1==0);
  }
  
  // remove request with given ID
  void remove(int k){
    // delete the node corresponding to this ID
    array.deleteNode(positions[k]);
    size1--;
  }
}; 
#endif
