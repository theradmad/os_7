// list/list.c
// 
// Implementation for linked list.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "list.h"

list_t *list_alloc() { 
  list_t* list = (list_t*)malloc(sizeof(list_t));
  list->head = NULL;
  return list; 
}

node_t *node_alloc(block_t *blk) {   
  node_t* node = malloc(sizeof(node_t));
  node->next = NULL;
  node->blk = blk;
  return node; 
}

void list_free(list_t *l){
  free(l);
  return;
}

void node_free(node_t *node){
  free(node);
  return;
}

void list_print(list_t *l) {
  node_t *current = l->head;
  block_t *b;
  
  if (current == NULL){
    printf("list is empty\n");
  }
  while (current != NULL){
    b = current->blk;
    printf("PID=%d START:%d END:%d", b->pid, b->start, b->end);
    current = current->next;
  }
}

int list_length(list_t *l) { 
  node_t *current = l->head;
  int i = 0;
  while (current != NULL){
    i++;
    current = current->next;
  }
  
  return i; 
}

void list_add_to_back(list_t *l, block_t *blk){  
  node_t* newNode = node_alloc(blk);
  newNode->next = NULL;
  if(l->head == NULL){
    l->head = newNode;
  }
  else{
    node_t *current = l->head;
    while(current->next != NULL){
      current = current->next;
    }
    current->next = newNode;
  }
}

void list_add_to_front(list_t *l, block_t *blk){  
  node_t* newNode = node_alloc(blk);
 
  newNode->next = l->head;
  l->head = newNode;
}

void list_add_at_index(list_t *l, block_t *blk, int index){
  int i = 0;
  
  node_t *newNode = node_alloc(blk);
  node_t *current = l->head;

  if(index == 0){
    newNode->next = l->head->next;
    l->head = newNode;
  }
  else if(index > 0){
    while(i < index && current->next != NULL){
      current = current->next;
      i++;
    }
  newNode->next = current->next;
  current->next = newNode;
  }
}

void list_add_ascending_by_blocksize(list_t *l, block_t *newblk) {
    node_t *newNode = node_alloc(newblk);
    node_t *current = l->head;
    node_t *prev = NULL;

    while (current != NULL && compareSize((newblk->end - newblk->start) + 1, current->blk)) {
        prev = current;
        current = current->next;
    }

    if (prev == NULL) {
        // Insert at the beginning
        newNode->next = l->head;
        l->head = newNode;
    } else {
        // Insert after the previous node
        prev->next = newNode;
        newNode->next = current;
    }
}


void list_add_ascending_by_address(list_t *l, block_t *newblk) {
    node_t *newNode = node_alloc(newblk);
    node_t *current = l->head;
    node_t *prev = NULL;

    while (current != NULL && newblk->start > current->blk->start) {
        prev = current;
        current = current->next;
    }

    if (prev == NULL) {
        // Insert at the beginning
        newNode->next = l->head;
        l->head = newNode;
    } else {
        // Insert after the previous node
        prev->next = newNode;
        newNode->next = current;
    }
}

void list_add_descending_by_blocksize(list_t *l, block_t *blk){
  node_t *current;
  node_t *prev;
  node_t *newNode = node_alloc(blk);
  int newblk_size = blk->end - blk->start;
  int curblk_size;
  
  if(l->head == NULL){
    l->head = newNode;
  }
  else{
    prev = current = l->head;
    
    curblk_size = current->blk->end - current->blk->start + 1;
    
    if(current->next == NULL) {  //only one node in list
       if(newblk_size >= curblk_size) {  // place in front of current node
          newNode->next = l->head;
          l->head = newNode;
       }
       else {   // place behind current node
          current->next = newNode;
          newNode->next = NULL;
       }
    }
    else {  // two or more nodes in list
      
       if(newblk_size >= curblk_size) {  // place in front of current node
          newNode->next = l->head;
          l->head = newNode;
       }
       else {
      
          while(current != NULL && newblk_size <= curblk_size) {
               prev = current;
               current = current->next;
               
               if(current != NULL)  // the last one in the list
                     curblk_size = current->blk->end - current->blk->start;
          }
          prev->next = newNode;
          newNode->next = current;
       }
    }
  }
}






block_t* list_remove_from_back(list_t *l){
  block_t *value = NULL;
  node_t *current = l->head;

  if(l->head != NULL){
    
    if(current->next == NULL) { // one node
         l->head->next = NULL;
         value = current->blk;
         node_free(current);
    }
    else {
         while (current->next->next != NULL){
            current = current->next;
         }
         value = current->blk;
         node_free(current->next);
         current->next = NULL;
    }
  }
  return value;
}

block_t* list_get_from_front(list_t *l) {
  block_t *value = NULL;
  if(l->head == NULL){
    return value;
  }
  else{
    node_t *current = l->head;
    value = current->blk;
  }
  return value; 
}


block_t* list_remove_from_front(list_t *l) { 
  block_t *value = NULL;
  if(l->head == NULL){
    return value;
  }
  else{
    node_t *current = l->head;
    value = current->blk;
    l->head = l->head->next;
    node_free(current);
  }
  return value; 
}

block_t* list_remove_at_index(list_t *l, int index) { 
  int i;
  block_t* value = NULL;
  
  bool found = false;
  
  if(l->head == NULL){
    return value;
  }
  else if (index == 0){
    return list_remove_from_front(l);
  }
  else if (index > 0){
    node_t *current = l->head;
    node_t *prev = current;
    
    i = 0;
    while(current != NULL && !found){
      if(i == index)
          found = true;
      else {
         prev = current;
         current = current->next;
         i++;
      }
    }
    
    if(found) {
      value = current->blk; 
      prev->next = current->next;
      node_free(current);
    }
  }
  return value; 
}

block_t *list_find_first_fit(list_t *l, int size) {
    node_t *current = l->head;

    while (current != NULL) {
        if (current->blk->end - current->blk->start + 1 >= size) {
            return current->blk;
        }
        current = current->next;
    }

    return NULL;
}

block_t *list_find_best_fit(list_t *l, int size) {
    // This is a placeholder implementation, you need to adapt it based on your specific best fit criteria
    node_t *current = l->head;
    block_t *bestFitBlock = NULL;
    int bestFitSize = INT_MAX;

    while (current != NULL) {
        int blockSize = current->blk->end - current->blk->start + 1;
        if (blockSize >= size && blockSize < bestFitSize) {
            bestFitBlock = current->blk;
            bestFitSize = blockSize;
        }
        current = current->next;
    }

    return bestFitBlock;
}

block_t *list_find_worst_fit(list_t *l, int size) {
    // This is a placeholder implementation, you need to adapt it based on your specific worst fit criteria
    node_t *current = l->head;
    block_t *worstFitBlock = NULL;
    int worstFitSize = 0;

    while (current != NULL) {
        int blockSize = current->blk->end - current->blk->start + 1;
        if (blockSize >= size && blockSize > worstFitSize) {
            worstFitBlock = current->blk;
            worstFitSize = blockSize;
        }
        current = current->next;
    }

    return worstFitBlock;
}

void list_remove_node(list_t *l, block_t *blk) {
    node_t *current = l->head;
    node_t *prev = NULL;

    while (current != NULL) {
        if (compareBlks(blk, current->blk)) {
            if (prev == NULL) {
                // Node to be removed is the head
                l->head = current->next;
            } else {
                prev->next = current->next;
            }
            node_free(current);
            return;
        }

        prev = current;
        current = current->next;
    }
}



bool compareBlks(block_t* a, block_t *b) {
  
  if(a->pid == b->pid && a->start == b->start && a->end == b->end)
     return true;
  
  return false;
}

bool compareSize(int a, block_t *b) {
    return a <= (b->end - b->start) + 1;
}

bool comparePid(int a, block_t *b) {
  
  if(a == b->pid)
     return true;
  
  return false;
}


bool list_is_in(list_t *l, block_t* value) { 
  node_t *current = l->head;
  while(current != NULL){
    if(compareBlks(value, current->blk)){
      return true;
    }
    current = current->next;
  }
return false; 
}

block_t* list_get_elem_at(list_t *l, int index) { 
 int i;
  block_t* value = NULL;
  if(l->head == NULL){
    return value;
  }
  else if (index == 0){
    return list_get_from_front(l);
  }
  else if (index > 0){
    node_t *current = l->head;
    
    i = 0;
    while(current != NULL){
      if(i == index)
          return(current->blk);
      else {
         current = current->next;
         i++;
      }
    }
  }
  return value; 
}

int list_get_index_of(list_t *l, block_t* value){
 int i = 0;
 node_t *current = l->head;
 if(l->head == NULL){
    return -1;
  }
  
  while (current != NULL){
   if (compareBlks(value,current->blk)){
     return i;
    }
    current = current->next;
    i++;
  }
  return -1; 
}

bool list_is_in_by_size(list_t *l, int Size){ 
  node_t *current = l->head;
  while(current != NULL){
    if(compareSize(Size, current->blk)){
      return true;
    }
    current = current->next;
  }
return false; 
}

bool list_is_in_by_pid(list_t *l, int pid) {
    node_t *current = l->head;

    while (current != NULL) {
        if (comparePid(pid, current->blk)) {
            return true;
        }
        current = current->next;
    }

    return false;
}

int list_get_index_of_by_Size(list_t *l, int Size){
 int i = 0;
 node_t *current = l->head;
 if(l->head == NULL){
    return -1;
  }
  
  while (current != NULL){
   if (compareSize(Size,current->blk)){
     return i;
    }
    current = current->next;
    i++;
  }

  return -1; 
}
                   
/* Returns the index at which the pid appears. */
int list_get_index_of_by_Pid(list_t *l, int pid){
 int i = 0;
 node_t *current = l->head;
 if(l->head == NULL){
    return -1;
  }
  
  while (current != NULL){
   if (comparePid(pid,current->blk)){
     return i;
    }
    current = current->next;
    i++;
  }

  return -1; 
}