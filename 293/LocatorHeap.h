#ifndef BH_H
#define BH_H
#include <vector>
#include <iostream>
#include "Event.h"
using namespace std;


template <class Comparable>
class LocatorHeap{

  
 private:
  int currentSize;          // number of elements in heap
  vector<Comparable> array; // vector containing elements in heap
  vector<int> positions;    // vector to store positions of an ID
    
  //Internal method to percolate down in the heap.
  //hole is the index at which the percolate begins.
  //Code taken from Lecture slides
  void percolateDown(int hole){
    int child;
    Comparable tmp=array[hole];
    for( ;hole*2 <= currentSize; hole=child){
      child=hole*2; // find the child element of hole
      // move the smaller child up to hole
      if(child!=currentSize && array[child + 1] <array[child])
        child++; 
      if( array[child] < tmp){
        // set smaller element in hole
        array[hole] =array[child];
        // update the positions vector accordingly
        positions[array[hole].getID()]=hole;
      }
      else
        break;
    }
    //finally set the required element in its correct position
    array[hole]=tmp;
    //update positions vector
    positions[array[hole].getID()]=hole;
  }
  
  //Internal method to percolate up in the heap.
  //hole is the index at which the percolate begins.  
  void percolateUp(int hole){
    Comparable x=array[hole];
    for(; hole> 1 && x < array[ hole / 2 ]; hole/=2){   
      // if parent is smaller than hole, then move the hole
      array[hole]=array[ hole / 2] ;
      // update positions vector
      positions[array[hole].getID()]=hole;
    }
    //finally set the required element in its currect position
    array[hole]=x;
    // update positions vector
    positions[array[hole].getID()]=hole;
  }
  
  
 public:
  
  // Constructor function
  LocatorHeap() {
    currentSize=0; 
    array.resize(1);
  }
  
  // Return the min element
  const Comparable& top(){
    return array[1];
  }
  
  // Check if the heap is empty
  bool empty(){ 
    return (currentSize==0);
  }
  
  // Return current size of heap
  int size(){
    return currentSize;
  }
  
  // Set maximum size of positions vector
  // It was required to be statically allocated according to problem statement
  void setnmax(int nmax){
    positions.resize(nmax);
  }
  
  // Push an element into heap, and put it in appropriate place
  // Code taken from lecture slides
  void push(const Comparable & x){
    // if array size is small, increase the size
    if(currentSize==array.size() - 1 )
      array.resize(array.size() * 2 );
      
    //This will be required when position vector size is not statically allocated
    /*if (positions.size()<=x.getID()){
     positions.resize(x.getID() +1);
    }*/
    
    // Percolate up
    int hole=++currentSize;
    for(; hole> 1 && x < array[ hole / 2 ]; hole/=2){   
     // if parent is smaller than hole, then move the hole
      array[hole]=array[ hole / 2] ;
      // update positions vector
      positions[array[hole].getID()]=hole;
    }
    //finally set the required element in its currect position
    array[hole]=x;
    // update positions vector
    positions[array[hole].getID()]=hole;
  }
  
  //Remove the minimum item
  void pop(){
    //if(isEmpty())
    //throw UnderflowException();
    array[1] = array[ currentSize-- ];
    positions[array[1].getID()]=1;
    percolateDown(1);
  }


  //Establish heap order property from an arbitrary
  //arrangement of items. Runs in linear time.
  void buildHeap( ){
    for( int i = currentSize / 2; i>0; i-- )
      percolateDown( i );
  }
  
  // change the key of an element with given ID to value 'newkey'
  void changeKey(int ID, double newKey){
    //get position of element 
    int n=positions[ID];
    //get key of element
    double k=array[n].getKey();
    
    //increase or decrease the value of key accordingly
    if (k>newKey) {decreaseKey(n,k-newKey);}
    else if (k<newKey) {increaseKey(n,newKey-k);}
  }
  
  // increase key of element at position 'pos' by 'value'
  void increaseKey(int pos, double value){
    //set new key value
    array[pos].setKey(array[pos].getKey()+value);
    //since now key has increased, this element will percolate down
    percolateDown(pos);
  }
  
  // decrease key of element at position 'pos' by 'value'
  void decreaseKey(int pos, double value){
    // set new key value
    array[pos].setKey(array[pos].getKey()-value);
    // since now key has decreased, this element will percolate up
    percolateUp(pos);
  }
  
  // remove the element with given ID and update heap and positions
  void remove(int id){
    // find position of given ID
    int pos=positions[id];
    // set last element to this position
    array[pos] = array[ currentSize-- ];
    positions[array[pos].getID()]=pos;
    // percolate last element down
    percolateDown(pos);
  }
};


#endif
