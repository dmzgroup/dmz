#ifndef DMZ_TYPES_DELETE_LIST_TEMPLATE_DOT_H
#define DMZ_TYPES_DELETE_LIST_TEMPLATE_DOT_H

namespace dmz {

template <class T>
void delete_list (T *&ptr) {

   while (ptr) {

      T *current = ptr;
      ptr = ptr->next;
      current->next = 0;
      delete current; current = 0;
   }
}

};

#endif // DMZ_TYPES_DELETE_LIST_TEMPLATE_DOT_H
