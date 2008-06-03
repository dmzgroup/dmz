#include <dmzTypesMask.h>
#include <stdlib.h>
#include <string.h>

/*!

\class dmz::Mask
\ingroup Types
\brief Dynamically sized bit mask.
\details The dmz::Mask is a bit mask of dynamic size. Rather than be limited to 32
or 64 bit flags as with typical bit masks that use unsigned integers, a dmz::Mask
is able to accommodate as may bit flags as memory will allow. The dmz::Mask supports
most typical bitwise operations.

*/

struct dmz::Mask::State {

   Int32 size;
   UInt32 *mask;
   State () : size (0), mask (0) {;}
   ~State () { empty (); }
   void empty () { if (mask) { delete []mask; mask = 0; size = 0; } }
   void clear () { if (mask) { memset (mask, '\0', size * sizeof (UInt32)); } }

   void grow_buffer (const Int32 Size) {

      if ((Size >= 0) && (Size > size)) {

         empty ();
         mask = new UInt32[Size];
         if (mask) { size = Size; clear (); }
      }
      else { clear (); }
   }

   void resize (const Int32 Size) {

      if (!mask) { grow_buffer (Size); }
      else if (Size > size) {

         const Int32 OldSize (size);
         UInt32 *oldMask (mask);

         mask = new UInt32[Size];

         if (mask) {

            size = Size;
            clear ();

            memcpy (mask, oldMask, OldSize * sizeof (UInt32));
            delete []oldMask; oldMask = 0;
         }
         else { mask = oldMask; }
      }
      else {;} // do nothing
   }
};


/*!

\brief Default constructor.
\details No initial storage is allocated.

*/
dmz::Mask::Mask () : _state (*(new State)) {;}


//! Copy constructor.
dmz::Mask::Mask (const Mask &Value) : _state (*(new State)) { *this = Value; }


/*!

\brief Shift constructor.
\details This constructor creates a dmz::Mask with a single bit set and shifted by the
number of bits specified by \a Shift.
\param[in] Shift Number of bits to shift single bit in mask.

*/
dmz::Mask::Mask (const Int32 Shift) : _state (*(new State)) {

   if (Shift >= 0) { set_sub_mask (0, 0x01); *this << Shift; }
}


/*!

\brief Shift bit mask constructor.
\details This constructor creates a dmz::Mask with a mask value set and shifted by
the number of bits specified by \a Shift.
\param[in] Shift Number of bits to shift 32 bit mask.
\param[in] Value 32 bit mask to be shifted.

*/
dmz::Mask::Mask (const Int32 Shift, const UInt32 Value) : _state (*(new State)) {

   if (Shift >= 0) { set_sub_mask (0, Value); *this << Shift; }
}


//! Destructor. Deletes all allocated storage.
dmz::Mask::~Mask () { delete &_state; }


/*!

\brief Relational "equal to" operator.
\details This function only compares bit that are set. If the two dmz::Mask objects
have the same bits set but one has more storage than the other, they are still
considered equal.
\param[in] Value Right hand value.
\return Returns dmz::True if the two masks have the same bit flags set.

*/
dmz::Boolean
dmz::Mask::operator== (const Mask &Value) const {

   const Int32 MaxSize =
      (_state.size > Value._state.size) ? _state.size : Value._state.size;

   Boolean done (False);
   Boolean result (True);
   Int32 count (0);

   while (!done) {

      if (((count < _state.size) ? _state.mask[count] : 0) !=
            ((count < Value._state.size) ? Value._state.mask[count] : 0)) {

         done = True;
         result = False;
      }
      else { count++; if (count >= MaxSize) { done = True; } }
   }

   return result;
}


/*!

\brief Relational "equal to" operator.
\param[in] Value Right hand value.
\return Returns dmz::True if either of the two dmz::Mask objects have a bit set in one and
not the other.

*/
dmz::Boolean
dmz::Mask::operator!= (const Mask &Value) const {

   const Int32 MaxSize =
      (_state.size > Value._state.size) ? _state.size : Value._state.size;

   Boolean done (False);
   Boolean result (False);
   Int32 count (0);

   while (!done) {

      if (((count < _state.size) ? _state.mask[count] : 0) !=
            ((count < Value._state.size) ? Value._state.mask[count] : 0)) {

         done = True;
         result = True;
      }
      else { count++; if (count >= MaxSize) { done = True; } }
   }

   return result;
}


/*!

\brief Assignment operator.
\details Copies content of \a Value.
\param[in] Value Right hand value.
\return Returns reference to self.

*/
dmz::Mask &
dmz::Mask::operator= (const Mask &Value) {

   _state.grow_buffer (Value._state.size);

   if (_state.mask && Value._state.mask) {

      const Int32 MinSize (
         (_state.size > Value._state.size) ? Value._state.size : _state.size);

      memcpy (_state.mask, Value._state.mask, MinSize * sizeof (UInt32));
   }

   return *this;
}


/*!

\brief Logical not operator.
\return Returns dmz::True if there is no storage allocated or if none of the allocated
storage has a bit set.

*/
dmz::Boolean
dmz::Mask::operator! () const {

   Boolean result (True);
   Int32 count (0);
   Boolean done ((_state.size > 0) ? False : True);

   while (!done) {

      if (_state.mask[count]) { done = True; result = False; }
      else { count++; if(count >= _state.size) { done = True; } }
   }

   return result;
}


/*!

\brief Bitwise not operator.
\return Returns a dmz::Mask that is a bitwise not of the dmz::Mask object being
operated on.

*/
dmz::Mask
dmz::Mask::operator~ () const {

   Mask result (*this);
   UInt32 *mask = result._state.mask;

   if (mask) {

      const Int32 Size = result._state.size;
      for (Int32 ix = 0; ix < Size; ix++) { mask[ix] = ~mask[ix]; }
   }

   return result;
}


/*!

\brief Bitwise XOR assignment operator.
\param[in] Value dmz::Mask object to perform the XOR operation with.
\return Returns a reference to self.

*/
dmz::Mask &
dmz::Mask::operator^= (const Mask &Value) {

   if (Value._state.mask) {

      const Int32 ValueSize (Value._state.size);
      const UInt32 *ValueMask (Value._state.mask);
      UInt32 *oldMask (_state.mask);
      const Int32 OldSize (_state.size);
      const Int32 Size = (ValueSize > OldSize) ? ValueSize : OldSize;

      if (ValueSize > OldSize) {

         _state.mask = 0;
         _state.size = 0;
         _state.grow_buffer (ValueSize);
      }

      if (_state.mask) {

         for (Int32 ix = 0; ix < Size; ix++) {

            _state.mask[ix] =
               ((OldSize > ix) ? oldMask[ix] : 0) ^
                  ((ValueSize > ix) ? ValueMask[ix] : 0);
         }
      }

      if (_state.mask != oldMask) { delete []oldMask; }
   }

   return *this;
}


/*!

\brief Bitwise XOR operator.
\param[in] Value dmz::Mask object to perform the XOR operation with.
\return Returns a dmz::Mask that contains is a bitwise XOR between the two
dmz::Mask objects being operated on.

*/
dmz::Mask
dmz::Mask::operator^ (const Mask &Value) const {

   Mask result (*this);
   return result ^= Value;
}


/*!

\brief Bitwise AND assignment operator.
\param[in] Value dmz::Mask object to perform the AND operation.
\return Returns a reference to self.

*/
dmz::Mask &
dmz::Mask::operator&= (const Mask &Value) {

   const Int32 Size (_state.size);
   const Int32 ValueSize (Value._state.size);
   const UInt32 *ValueMask = Value._state.mask;

   for (Int32 ix = 0; ix < Size; ix++) {

      _state.mask[ix] &= (ValueSize > ix) ? ValueMask[ix] : 0;
   }

   return *this;
}


/*!

\brief Bitwise AND operator.
\param[in] Value dmz::Mask object to perform the AND operation.
\return Returns a dmz::Mask that contains is a bitwise AND between the two
dmz::Mask objects.

*/
dmz::Mask
dmz::Mask::operator& (const Mask &Value) const {

   Mask result (*this);
   return result &= Value;
}


/*!

\brief Bitwise OR assignment operator.
\param[in] Value dmz::Mask object to perform the OR operation.
\return Returns a reference to self.

*/
dmz::Mask &
dmz::Mask::operator|= (const Mask &Value) {

   if (Value._state.mask) {

      const Int32 ValueSize (Value._state.size);
      const UInt32 *ValueMask (Value._state.mask);
      UInt32 *oldMask (_state.mask);
      const Int32 OldSize (_state.size);
      const Int32 Size = (ValueSize > OldSize) ? ValueSize : OldSize;

      if (ValueSize > OldSize) {

         _state.mask = 0;
         _state.size = 0;
         _state.grow_buffer (ValueSize);
      }

      if (_state.mask) {

         for (Int32 ix = 0; ix < Size; ix++) {

            _state.mask[ix] =
               ((OldSize > ix) ? oldMask[ix] : 0) |
                  ((ValueSize > ix) ? ValueMask[ix] : 0);
         }
      }

      if (_state.mask != oldMask) { delete []oldMask; }
   }

   return *this;
}


/*!

\brief Bitwise OR operator.
\param[in] Value dmz::Mask object to perform the OR operation.
\return Returns a dmz::Mask that contains is a bitwise OR between the two
dmz::Mask objects.

*/
dmz::Mask
dmz::Mask::operator| (const Mask &Value) const {

   Mask result (*this);
   return result |= Value;
}


/*!

\brief Bitwise left shift operator.
\param[in] Shift Number of bits the mask is shifted to the left.
\return Returns a reference to self.

*/
dmz::Mask &
dmz::Mask::operator<< (const Int32 Shift) {

   if ((Shift > 0) && (_state.size >= 1)) {

      Boolean done (False);
      Int32 place (_state.size - 1);
      Int32 found (-1);

      while (!done) {

         if (_state.mask[place]) { found = place; done = True; }
         else { place--; if (place < 0) { done = True; } }
      }

      if (found >= 0) {

         const Int32 Offset (Shift / 32);
         const Int32 ElementShift (Shift % 32);
         const Int32 OverflowShift (32 - ElementShift);
         const UInt32 OverflowMask (0xffffffff << (OverflowShift));
         const UInt32 OverflowSize (_state.mask[found] & OverflowMask ? 1 : 0);
         const Int32 NewSize (OverflowSize + found + Offset + 1);
         UInt32 *newMask (new UInt32[NewSize]);

         if (newMask) {

            memset (newMask, '\0', NewSize * sizeof (UInt32));

            UInt32 prevOverflow (0);

            for (Int32 ix = 0; ix <= found; ix++) {

               const UInt32 Value (_state.mask[ix] << ElementShift);
               newMask[ix + Offset] = Value | prevOverflow;
               prevOverflow = (_state.mask[ix] & OverflowMask) >> OverflowShift;
            }

            if (prevOverflow) { newMask[found + Offset + 1] = prevOverflow; }

            _state.size = NewSize;
            delete []_state.mask; _state.mask = 0;
            _state.mask = newMask;
         }
      }
   }

   return *this;
}


/*!

\brief Bitwise right shift operator.
\details Bit that shift past the zero bit are lost.
\param[in] Shift Number of bits the mask is shifted to the right.
\return Returns a reference to self.

*/
dmz::Mask &
dmz::Mask::operator>> (const Int32 Shift) {

   if ((Shift > 0) && (_state.size >= 1)) {

      Boolean done (False);
      Int32 place (_state.size - 1);
      Int32 found (-1);

      while (!done) {

         if (_state.mask[place]) { found = place; done = True; }
         else { place--; if (place < 0) { done = True; } }
      }

      if (found >= 0) {

         const Int32 Offset (Shift / 32);
         const Int32 ElementShift (Shift % 32);
         const Int32 OverflowShift (32 - ElementShift);
         const UInt32 OverflowMask (0xffffffff >> (OverflowShift));
         const Int32 NewSize (found + 1 + (-Offset));

         if (NewSize > 0) {

            UInt32 *newMask (new UInt32[NewSize]);

            if (newMask) {

               memset (newMask, '\0', NewSize * sizeof (UInt32));

               UInt32 prevOverflow (0);

               for (Int32 ix = (NewSize - 1); ix >= 0; ix--) {

                  const UInt32 Value (_state.mask[ix + Offset] >> ElementShift);
                  newMask[ix] = Value | prevOverflow;
                  prevOverflow =
                     (_state.mask[ix + Offset] & OverflowMask) << OverflowShift;
               }

               _state.size = NewSize;
               delete []_state.mask; _state.mask = 0;
               _state.mask = newMask;
            }
         }
         else { memset (_state.mask, '\0', _state.size * sizeof (UInt32)); }
      }
   }


   return *this;
}


/*!

\brief Sets size of mask storage.
\details If the dmz::Mask contains a value, it will be retained after set_size is called.
This function can only grow the storage. If \a Size is smaller than the current
size of the dmz::Mask, the call is ignored. The internal storage is allocated in
32 bit block. So a \a Size of 4 will allocate 128 bit of storage.
\param[in] Size Size of mask storage.
\return Returns dmz::True if mask was successfully resized.

*/
dmz::Boolean
dmz::Mask::grow (const Int32 Size) {

   Boolean result (False);

   if (Size > _state.size) { _state.resize (Size); result = (Size <= _state.size); }
   else { result = True; }

   return result;
}


/*!

\brief Gets size of mask storage.
\return Returns number of 32 bit blocks allocated for mask storage.

*/
dmz::Int32
dmz::Mask::get_size () const { return _state.size; }


/*!

\brief Sets a 32 bit mask at the given offset within the mask.
\details This function will store a 32 bit mask specified by a 32 bit block offset.
If the offset extends past the current size of storage, the storage is grown
to accommodate the sub mask.
\param[in] Offset 32 bit block offset in to mask.
\param[in] Value 32 bit mask to be stored in mask.
\return Returns dmz::True if the 32 bit mask is successfully stored.

*/
dmz::Boolean
dmz::Mask::set_sub_mask (const Int32 Offset, const UInt32 Value) {

   Boolean result (False);

   _state.resize (Offset + 1);

   if (_state.mask && (Offset < _state.size)) {

      _state.mask[Offset] = Value;
      result = True;
   }

   return result;
}


/*!

\brief Gets a 32 bit mask at the given offset within the mask.
\details This function will return a 32 bit mask specified by a 32 bit block offset.
\param[in] Offset 32 bit block offset in to mask.
\return Returns a 32 bit mask at the offset specified.

*/
dmz::UInt32
dmz::Mask::get_sub_mask (const Int32 Offset) const {

   return (_state.mask && (Offset < _state.size)) ? _state.mask[Offset] : 0;
}


/*!

\brief Deletes mask storage.
\return Returns a reference to self.

*/
dmz::Mask &
dmz::Mask::empty () { _state.empty (); return *this; }


/*!

\brief Clears mask storage.
\details This function sets all bits in the mask to zero but does \b not delete any
storage.
\return Returns a reference to self.

*/
dmz::Mask &
dmz::Mask::clear () { _state.clear (); return *this; }


/*!

\brief Determines if any bits in the mask are set.
\return Returns dmz::True if any bit in the mask storage is set.

*/
dmz::Boolean
dmz::Mask::is_set () const {

   Boolean result (False);

   if (_state.mask) {

      Boolean done (False);
      Int32 count (0);

      while (!done) {

         if (_state.mask[count]) { result = True; done = True; }
         else { count++; if (count >= _state.size) { done = True; } }
      }
   }

   return result;
}


/*!

\brief Set a single bit in the mask
\details If the bit extends past the current size of storage, the storage is grown
to accommodate the new bit.
\param[in] Bit Bit to be set.
\return Returns reference to self.

*/
dmz::Mask &
dmz::Mask::set_bit (const Int32 Bit) {

   const div_t Result = div (int (Bit), 32);
   Int32 Place (Result.quot);
   Int32 Shift (Result.rem);

   _state.resize (Place + 1);
   if (_state.mask && (Place < _state.size)) { _state.mask[Place] |= 0x1 << Shift; }

   return *this;
}


/*!

\brief Unset a single bit in the mask
\param[in] Bit Bit to be unset.
\return Returns reference to self.

*/
dmz::Mask &
dmz::Mask::unset_bit (const Int32 Bit) {

   const div_t Result = div (int (Bit), 32);
   Int32 Place (Result.quot);
   Int32 Shift (Result.rem);

   if (_state.mask && (Place < _state.size)) { _state.mask[Place] &= ~(0x1 << Shift); }

   return *this;
}


/*!

\brief Gets the value of a single bit in the mask
\return Returns dmz::True if the bit is set.

*/
dmz::Boolean
dmz::Mask::get_bit (const Int32 Bit) const {

   Boolean result (False);
   const div_t Result = div (int (Bit), 32);
   Int32 Place (Result.quot);
   Int32 Shift (Result.rem);

   if (_state.mask && (Place < _state.size)) {

      if (_state.mask[Place] & (0x01 << Shift)) { result = True; }
   }

   return result;
}

/*!

\fn dmz::Mask::contains (const Mask &Value) const
\brief Determines if the passed in mask is contained with in the mask.
\details This function test if the passed in mask \a Value is contained with in the mask
storage.
\param[in] Value Sub mask to test.
\return Returns dmz::True if the passed in mask is contained in the mask storage.

*/

/*!

\brief Unset specified mask.
\param[in] Value Mask to unset.
\return Returns reference to self.

*/
dmz::Mask &
dmz::Mask::unset (const Mask &Value) {

   const Int32 Size (_state.size);
   const Int32 ValueSize (Value._state.size);
   const UInt32 *Mask = Value._state.mask;

   for (Int32 ix = 0; ix < Size; ix++) {

      _state.mask[ix] &= (ValueSize > ix) ? ~(Mask[ix]) : 0xFFFFFFFF;
   }

   return *this;
}

