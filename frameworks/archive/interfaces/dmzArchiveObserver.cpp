/*!

\class dmz::ArchiveObserver
\ingroup Archive
\brief Archive observer interface.
\details

\fn dmz::ArchiveObserver *dmz::ArchiveObserver::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Casts Plugin pointer to an ArchiveObserver.
\details If the Plugin ojbect implements the ArchiveObserver interface, a pointer
to the ArchiveObserver interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin.
\param[in] PluginName String containing name of desired Observer. Will not test for name
if \a PluginName is an empty string.
\return Returns a pointer to the ArchiveObserver. Returns NULL if the PluginPtr does not
implement the ArchiveObserver interface or the \a PluginName is not empty and not equal to
the Plugin's name.

\fn dmz::ArchiveObserver::ArchiveObserver (const PluginInfo &Info)
\brief Constructor.
\param[in] Info PluginInfo containing initialization data.

\fn dmz::ArchiveObserver::~ArchiveObserver ()
\brief Destructor.

\fn dmz::Handle dmz::ArchiveObserver::get_archive_observer_handle ()
\brief Gets archive observer's unique runtime Handle.
\return Returns the archive observer's runtime Handle.

\fn dmz::String dmz::ArchiveObserver::get_archive_observer_name ()
\brief Gets the archive observer's name.
\return Returns a String containing the name of the archive observer.

\fn dmz::StringContainer dmz::ArchiveObserver::get_archive_scope (
const Handle ArchiveHandle)
\brief Returns plugin Config scope for the given handle.
\param[in] ArchiveHandle Handle of the archive group.
\return Returns a dmz::StringContainer with the supported Config scopes for the 
archive observer.
\fn void dmz::ArchiveObserver::store_archive_module (
const String &Name,
ArchiveModule &module)
\brief Stores the archive module in the observer.
\param[in] Name String containing the name of the module.
\param[in] module ArchiveModule reference.

\fn void dmz::ArchiveObserver::remove_archive_module (
const String &Name,
ArchiveModule &module)
\brief Removes the archive module from the observer.
\param[in] Name String containing the name of the module.
\param[in] module ArchiveModule reference.

\fn void dmz::ArchiveObserver::pre_create_archive (const Handle ArchiveHandle, const Int32 Version)
\brief Pre create archive callback.
\details Callback is made before the dmz::ArchiveObserver::create_archive() callback.
Any pre-processing that needs to be performed before the archive is created should
be done at this time.
\param[in] ArchiveHandle Handle of the archive group.
\param[in] Version Int32 of the archive's version number. If the version number is less
than zero, no version number has been defined.

\fn void dmz::ArchiveObserver::create_archive (
const Handle ArchiveHandle,
const Int32 Version,
Config &local,
Config &global)
\brief Create archive callback.
\details Data to be archived should be stored in the \a local Config. The \a global
Config may be used if global data needs to be archived but should generally be avoided.
\param[in] ArchiveHandle Handle of the archive group.
\param[in] Version Int32 of the archive's version number. If the version number is less
than zero, no version number has been defined.
\param[out] local Config used to store archive data.
\param[out] global Config used to store global archive data.

\fn void dmz::ArchiveObserver::post_create_archive (const Handle ArchiveHandle, const Int32 Version)
\brief Post create archive callback.
\details Callback is made after the dmz::ArchiveObserver::create_archive() callback.
Any post-processing that needs to be performed after the archive is created should
be done at this time.
\param[in] ArchiveHandle Handle of the archive group.
\param[in] Version Int32 of the archive's version number. If the version number is less
than zero, no version number has been defined.

\fn void dmz::ArchiveObserver::pre_process_archive (const Handle ArchiveHandle, const Int32 Version)
\brief Pre process archive callback.
\details Callback is made before the dmz::ArchiveObserver::process_archive() callback.
Any pre-processing that needs to be performed before the archive is processed should
be done at this time.
\param[in] ArchiveHandle Handle of the archive group.
\param[in] Version Int32 of the archive's version number. If the version number is less
than zero, no version number has been defined.

\fn void dmz::ArchiveObserver::process_archive (
const Handle ArchiveHandle,
const Int32 Version,
Config &local,
Config &global)
\brief Process archive callback.
\details Archived data is stored in the \a local Config. The \a global
Config contains global archive data.
\param[in] ArchiveHandle Handle of the archive group.
\param[in] Version Int32 of the archive's version number. If the version number is less
than zero, no version number has been defined.
\param[in] local Config containing archive data.
\param[in] global Config containing global archive data.

\fn void dmz::ArchiveObserver::post_process_archive (const Handle ArchiveHandle, const Int32 Version)
\brief Post process archive callback.
\details Callback is made after the dmz::ArchiveObserver::process_archive() callback.
Any post-processing that needs to be performed after the archive is processed should
be done at this time.
\param[in] ArchiveHandle Handle of the archive group.
\param[in] Version Int32 of the archive's version number. If the version number is less
than zero, no version number has been defined.

*/
