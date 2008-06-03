#ifndef DMZ_TYPES_MASK_DOT_H
#define DMZ_TYPES_MASK_DOT_H

#include <dmzTypesBase.h>
#include <dmzTypesBooleanOperator.h>
#include <dmzKernelExport.h>

namespace dmz {

   class DMZ_KERNEL_LINK_SYMBOL Mask {

      public:
         Mask ();
         Mask (const Mask &Value);
         Mask (const Int32 Shift);
         Mask (const Int32 Shift, const UInt32 Value);
         ~Mask ();

         Boolean operator== (const Mask &Value) const;
         Boolean operator!= (const Mask &Value) const;
         Mask &operator= (const Mask &Value);
         Boolean operator! () const;
         DMZ_BOOLEAN_OPERATOR;
         Mask operator~ () const;
         Mask &operator^= (const Mask &Value);
         Mask operator^ (const Mask &Value) const;
         Mask &operator&= (const Mask &Value);
         Mask operator& (const Mask &Value) const;
         Mask &operator|= (const Mask &Value);
         Mask operator| (const Mask &Value) const;

         Mask &operator<< (const Int32 Shift);
         Mask &operator>> (const Int32 Shift);

         Boolean grow (const Int32 Size);
         Int32 get_size () const;
         Boolean set_sub_mask (const Int32 Offset, const UInt32 Value);
         UInt32 get_sub_mask (const Int32 Offset) const;

         Mask &empty ();
         Mask &clear ();
         Boolean is_set () const;
         Mask &set_bit (const Int32 Bit);
         Mask &unset_bit (const Int32 Bit);
         Boolean get_bit (const Int32 Bit) const;
         Boolean contains (const Mask &Value) const { return (*this & Value) == Value; }
         Mask &unset (const Mask &Value);

      protected:
         struct State;
         State &_state; //!< Internal state.
   };
};

#endif // DMZ_TYPES_MASK_DOT_H
