/*

WARNING!!! WARNING!!! WARNING!!! WARNING!!! WARNING!!!
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=
DO NOT INCLUDE THIS HEADER!!! IT IS STRICTLY FOR DOCUMENTATION PURPOSES.
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=
WARNING!!! WARNING!!! WARNING!!! WARNING!!! WARNING!!!

*/

#error "Header intended for documentation purposes only"

namespace dmz {
/*!

\brief Runtime context.
\ingroup Runtime
\details The runtime context class is an internal type to the runtime.
A pointer to this class is often used to initialize most runtime classes.
A valid runtime context pointer may typically be obtained from a dmz::PluginInfo
object.

*/
class RuntimeContext {};

/*!
\brief Config context.
\ingroup Runtime
\details The config context is a runtime internal class that contains configuration data.
The config context may be accessed and manipulated through the dmz::Config class.

*/
class ConfigContext {};

/*!

\brief Type context.
\ingroup Runtime
\details The type context is a runtime internal class that is used by both the
dmz::EventType and dmz::ObjectType classes as their internal implementation.

*/
class TypeContext {};

/*!

\brief Message type context.
\ingroup Runtime
\details The message type context is a runtime internal class that represents message
types in the runtime. The message type context may have one parent message type context
and multiple children message type contexts. When subscribing to a message,
the subscriber will also receive all children message types that are sent globally or
directly to the subscriber.

*/
class MessageContext {};

};
