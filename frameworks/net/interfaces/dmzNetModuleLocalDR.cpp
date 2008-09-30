/*!

\class dmz::NetModuleLocalDR
\ingroup Net
\brief Calculates if the attributes of a local object have changed sufficiently to cause
a network packet to be generated.

\fn dmz::NetModuleLocalDR::NetModuleLocalDR (const PluginInfo &Info)
\brief Constructor.

\fn dmz::NetModuleLocalDR::~NetModuleLocalDR ()
\brief Destructor.

\fn dmz::NetModuleLocalDR *dmz::NetModuleLocalDR::cast (
const Plugin *PluginPtr,
const String &PluginName);
\brief Casts Plugin pointer to an NetModuleLocalDR.
\details If the Plugin object implements the NetModuleLocalDR interface, a pointer to
the NetModuleLocalDR interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired NetModuleLocalDR.
\return Returns pointer to the NetModuleLocalDR. Returns NULL if the PluginPtr does not
implement the NetModuleLocalDR interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::Boolean dmz::NetModuleLocalDR::update_object (const Handle ObjectHandle)
\brief Test if an object's current attribute values require a network transmission.
\param[in] ObjectHandle Handle of object to test.

*/
