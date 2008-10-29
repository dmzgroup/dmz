/*!

\class dmz::Volume
\ingroup Types
\brief Volume base class.
\details Provides an interface for testing if a point is contained in a volume.
 
\fn void dmz::Volume::set_origin (const Vector &Origin)
\brief Sets the origin of the volume.
\param[in] Origin Vector specifying the origin of the Volume.

\fn dmz::Vector dmz::Volume::get_origin () const
\brief Gets the origin of the Volume.
\return Returns a Vector containing the origin of the Volume.

\fn dmz::Boolean dmz::Volume::contains_point (const Vector &Point) const 
\brief Test if the Point is contained in the Volume.
\param[in] Point Vector containing point to test.

\fn void dmz::Volume::get_extents (Vector &origin, Vector &min, Vector &max) const
\brief Gets the bounding box of the Volume.
\param[out] origin Vector containing the origin of the volume.
\param[out] min Vector containing the minimum values of the bounding box.
\param[out] max Vector containing the maximum values of the bounding box.

*/
