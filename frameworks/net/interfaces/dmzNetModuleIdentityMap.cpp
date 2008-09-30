/*!

\class dmz::NetModuleIdentityMap
\ingroup Net
\brief Provides a mapping between internal and external object identification.

\fn dmz::NetModuleIdentityMap *dmz::NetModuleIdentityMap::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Casts Plugin pointer to an NetModuleIdentityMap.
\details If the Plugin object implements the NetModuleIdentityMap interface, a pointer to
the NetModuleIdentityMap interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired NetModuleIdentityMap.
\return Returns pointer to the NetModuleIdentityMap. Returns NULL if the PluginPtr does
not implement the NetModuleIdentityMap interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::NetModuleIdentityMap::NetModuleIdentityMap (const PluginInfo &Info)
\brief Constructor.

\fn dmz::NetModuleIdentityMap::~NetModuleIdentityMap ()
\brief Destructor.

\fn dmz::UInt32 dmz::NetModuleIdentityMap::get_site_id ()
\brief Gets site id for current simulation instance.

\fn dmz::UInt32 dmz::NetModuleIdentityMap::get_host_id ()
\brief Gets host id for current simulation instance.

\fn dmz::Boolean dmz::NetModuleIdentityMap::create_site_host_entity (
const Handle ObjectHandle,
UInt32 &site,
UInt32 &host,
UInt32 &entity)
\brief Creates a new site/host/entity id and binds it to an object Handle.
\param[in] ObjectHandle Handle of object to create new site/host/entity.
\param[out] site Id of site.
\param[out] host Id of host.
\param[out] entity Id of entity.
\return Returns dmz::True if a new site/host/entity id was created.

\fn dmz::Boolean dmz::NetModuleIdentityMap::store_site_host_entity (
const Handle ObjectHandle,
const UInt32 Site,
const UInt32 Host,
const UInt32 Entity)
\brief Binds a site/host/entity to and object Handle.
\param[in] ObjectHandle handle of object to bind.
\param[in] Site Id of site to bind to object.
\param[in] Host Id of host to bind to object.
\param[in] Entity Id of entity to bind to object.
\return Returns dmz::True if the site/host/entity was successfully bound to the object.

\fn dmz::Boolean dmz::NetModuleIdentityMap::lookup_site_host_entity (
const Handle ObjectHandle,
UInt32 &site,
UInt32 &host,
UInt32 &entity)
\brief Looks up site/host/entity bound to object Handle.
\param[in] ObjectHandle Handle of the object.
\param[out] site Id of site.
\param[out] host Id of host.
\param[out] entity Id of entity.
\return Returns dmz::True if a site/host/entity was found for the object.

\fn dmz::Boolean dmz::NetModuleIdentityMap::store_name (
const Handle ObjectHandle,
const String &Name)
\brief Bind name to object.
\param[in] ObjectHandle Handle of the object.
\param[in] Name String containing the name to bind to the object.

\fn dmz::Boolean dmz::NetModuleIdentityMap::lookup_name (
const Handle ObjectHandle,
String &name)
\brief Looks up name bound to object.
\param[in] ObjectHandle Handle of the object.
\param[out] name String containing the name bound to the object.
\return Returns dmz::True if a name was bound to the object.

\fn dmz::Boolean dmz::NetModuleIdentityMap::lookup_handle_from_name (
const String &Name,
Handle &handle)
\brief Looks up object handle bound to name.
\param[in] Name String containing object name.
\param[out] handle Handle containing bound object.
\return Returns dmz::True if an object was bound to the name.

\fn dmz::Boolean dmz::NetModuleIdentityMap::lookup_handle_from_site_host_entity (
const UInt32 Site,
const UInt32 Host,
const UInt32 Entity,
Handle &handle)
\brief Looks up object bound to site/host/entity.
\param[in] Site Id of the site.
\param[in] Host Id of the host.
\param[in] Entity Id of the entity.
\param[out] handle Handle of the bound object.
\return Returns dmz::True if an object was bound to the site/host/entity.

\fn dmz::Boolean dmz::NetModuleIdentityMap::remove_object (const Handle ObjectHandle)
\brief Remove all bindings to a given object.
\param[in] ObjectHandle Handle of object to unbind.
\return Returns dmz::True if the object was successfully unbound.

*/
