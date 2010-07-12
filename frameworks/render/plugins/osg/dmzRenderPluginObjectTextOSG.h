#ifndef DMZ_RENDER_PLUGIN_OBJECT_TEXT_OSG_DOT_H
#define DMZ_RENDER_PLUGIN_OBJECT_TEXT_OSG_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeResources.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesVector.h>

#include <osg/Geode>
#include <osgText/Text>

namespace dmz {

   class RenderModuleCoreOSG;

   class RenderPluginObjectTextOSG :
         public Plugin,
         public ObjectObserverUtil {

      public:
         RenderPluginObjectTextOSG (const PluginInfo &Info, Config &local);
         ~RenderPluginObjectTextOSG ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

         virtual void update_object_text (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const String &Value,
            const String *PreviousValue);

      protected:
         // RenderPluginObjectTextOSG Interface
         struct TextDefStruct {

            const Handle Attribute;
            const osg::Vec4 Color;
            const Vector Offset;
            const String FontName;
            const Float64 Height;

            TextDefStruct (
                  const Handle TheAttribute,
                  const osg::Vec4 &TheColor,
                  const Vector &TheOffset,
                  const String &TheFontName,
                  const Float64 TheHeight) :
                  Attribute (TheAttribute),
                  Color (TheColor),
                  Offset (TheOffset),
                  FontName (TheFontName),
                  Height (TheHeight) {;}
         };

         struct TypeStruct {

            HashTableHandleTemplate<TextDefStruct> table;
         };

         struct TextStruct {

            const TextDefStruct &Def;
            osg::ref_ptr<osgText::Text> text;
            String data;
            osg::Vec4 color;

            TextStruct (const TextDefStruct &TheDef) :
               Def (TheDef),
               color (TheDef.Color) {;}
         };

         struct ObjectStruct {

            const Handle Object;
            const TypeStruct &Defs;
            HashTableHandleTemplate<TextStruct> table;

            osg::ref_ptr<osg::Geode> geode;

            ObjectStruct (const Handle TheObject, const TypeStruct &TheDefs) :
                  Object (TheObject),
                  Defs (TheDefs) {;}

            ~ObjectStruct () { table.empty (); }
         };

         void _update_text (ObjectStruct &os, TextStruct &ts);
         void _create_text_struct (const Handle Attribute, ObjectStruct &os);
         TypeStruct *_lookup_type_struct (const ObjectType &Type);
         TypeStruct *_create_type_struct (const ObjectType &Type);
         void _init (Config &local);

         Log _log;
         Resources _rc;

         RenderModuleCoreOSG *_core;

         HashTableHandleTemplate<TypeStruct> _typeTable;
         HashTableHandleTemplate<TypeStruct> _typeMap;
         HashTableHandleTemplate<ObjectStruct> _objTable;

      private:
         RenderPluginObjectTextOSG ();
         RenderPluginObjectTextOSG (const RenderPluginObjectTextOSG &);
         RenderPluginObjectTextOSG &operator= (const RenderPluginObjectTextOSG &);

   };
};

#endif // DMZ_RENDER_PLUGIN_OBJECT_TEXT_OSG_DOT_H
