// BoundedBuffer
// boundedbuffer.h
// Created by: Ryan Walters
// Date Created: November 8, 2015

#ifndef _boundedbuffer_H_                   // include file only once
#define _boundedbuffer_H_

#include <string>
#include <queue>
#include "semaphore.h"

using namespace std;

class Response {
public:
    string data;
    int req_id;
    int req_number;
    Response(string d, int i, int n){
        data = d;
        req_id = i;
        req_number = n;
    }
};

class BoundedBuffer {
    // We want bounded buffer to be overflow safe(doesnt surpass bound, b)
    // These should not be constructed here.. Need to do this in the constructor when coding
private:
    Semaphore* full;
    Semaphore* empty;
    Semaphore* mut;
    
    int size;
    queue<Response> data; // Main data structure that we want to make thread-safe
    
public:
    BoundedBuffer() { }
    
    BoundedBuffer(int _size){
        int size = _size;
        full = new Semaphore(0);
        empty = new Semaphore(size);
        mut = new Semaphore(1);
        
    }
    
    ~BoundedBuffer(){
        delete full;
        delete empty;
        delete mut;
    }
    
    void push(Response item){
        empty->P(); // When this returns we know for a fact it is empty and ready for manipulation
        
        mut->P(); // Lock with semaphore of size 1, preventing others from using it
        data.push(item); // NOW we can safely change the data
        mut->V(); // Unlock to resume modifications
        
        full->V();
    }
    
    Response pop(){
        full->P();
        
        mut->P(); // Lock with semaphore of size 1, preventing others from using it
        Response output = data.front(); // NOW we can safely change the data
        data.pop();
        mut->V(); // Unlock to resume modifications
        
        empty->V();
        return output;
    }
};

#endif