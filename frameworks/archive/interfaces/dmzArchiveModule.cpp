/*!

\defgroup Archive DMZ Archive Framework

\class dmz::ArchiveModule
\ingroup Archive
\brief Archive module interface.
\details The ArchiveModule provides a single interface for creating and processing
archives of application data. The archives are both generated and processed by the
archive observers. These archive observers may register with various archive groups.
A group are defined by a named handle. When an archive is either created or processed
for a particular archive group, the registered archive observers are notified so that
they may process or create archive data.
\sa dmz::ArchiveObserver \n dmz::Definitions::create_named_handle()

\fn dmz::ArchiveModule *dmz::ArchiveModule::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Casts Plugin pointer to an ArchiveModule.
\details If the Plugin object implements the AtrchiveModule interface, a pointer to the
ArchiveModule interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin.
\param[in] PluginName String containing name of desired Module. Will not test for name
if \a PluginName is an empty string.
\return Returns a pointer to the ArchiveModule. Returns NULL if the PluginPtr does not
implement the ArchiveModule interface or the \a PluginName is not empty and not equal to
the Plugin's name.

\fn dmz::ArchiveModule::ArchiveModule (const PluginInfo &Info)
\brief Constructor.
\param[in] Info PluginInfo containing initialization data.

\fn dmz::ArchiveModule::~ArchiveModule ()
\brief Destructor.

\fn dmz::String dmz::ArchiveModule::get_archive_module_name () const
\brief Gets archive module's name.
\return Returns String containing archive module's name.

\fn dmz::Handle dmz::ArchiveModule::get_archive_module_handle () const
\brief Gets archive module's runtime Handle.
\return Returns Handle containing archive module's runtime Handle.

\fn dmz::Boolean dmz::ArchiveModule::register_archive_observer (
const Handle ArchiveHandle,
ArchiveObserver &observer)
\brief Register archive observer.
\details Allows an archive observer to register for archive creation and processing.
The same observer may be registered with different archives.
\param[in] ArchiveHandle Handle of the archive group.
\param[in] observer ArchiveObserver being registered with the group.
\return Return dmz::True if the observer was successfully register.

\fn dmz::Boolean dmz::ArchiveModule::release_archive_observer (
const Handle ArchiveHandle,
ArchiveObserver &observer)
\brief Releases archive observer.
\details Allows and archive observer to be released from archive creation and processing.
\param[in] ArchiveHandle Handle of the archive group.
\param[in] observer ArchiveObserver being released from the group.
\return Return dmz::True if the observer was successfully released.

\fn dmz::Config dmz::ArchiveModule::create_archive (const Handle ArchiveHandle)
\brief Creates an archive.
\details All ArchiveObserver that are registered with the archive group specified by
\a ArchvieHandle store archive data in a Config.
\param[in] ArchiveHandle Handle specifying the archive group to use in creating the
archive.
\return Returns a Config containing the archive data.

\fn void dmz::ArchiveModule::process_archive (const Handle ArchiveHandle, Config &archive)
\brief Processes an archive.
\details All ArchiveObserver that are registered with the archive group specified by
\a ArchvieHandle process the archive data stored in the Config \a archive.
\param[in] ArchiveHandle Handle specifying the archive group to used to process the
archive.
\param[in] archive Config containing the archive.

*/
