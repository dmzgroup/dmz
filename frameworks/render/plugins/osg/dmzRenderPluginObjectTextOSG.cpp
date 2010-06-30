#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzRenderConfigToOSG.h>
#include <dmzRenderUtilOSG.h>
#include <dmzRenderModuleCoreOSG.h>
#include "dmzRenderPluginObjectTextOSG.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::RenderPluginObjectTextOSG::RenderPluginObjectTextOSG (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _rc (Info),
      _core (0) {

   _init (local);
}


dmz::RenderPluginObjectTextOSG::~RenderPluginObjectTextOSG () {

   _typeMap.clear ();
   _typeTable.empty ();
   _objTable.empty ();
}


// Plugin Interface
void
dmz::RenderPluginObjectTextOSG::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::RenderPluginObjectTextOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) {

         _core = RenderModuleCoreOSG::cast (PluginPtr);

         if (_core) {

         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) {

         _core = 0;
      }
   }
}


// Object Observer Interface
void
dmz::RenderPluginObjectTextOSG::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   TypeStruct *ts =_lookup_type_struct (Type);

   if (ts) {

      ObjectStruct *os = new ObjectStruct (ObjectHandle, *ts);

      if (os && _objTable.store (ObjectHandle, os)) {

         HashTableHandleIterator it;
         TextDefStruct *tds (0);

         while (ts->table.get_next (it, tds)) {

            _create_text_struct (tds->Attribute, *os);
         }
      }
      else if (os) { delete os; os = 0; }
   }
}


void
dmz::RenderPluginObjectTextOSG::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   ObjectStruct *os = _objTable.remove (ObjectHandle);

   if (os) {

      if (os->geode.valid () && _core) {

         osg::ref_ptr<osg::Group> root = _core->lookup_dynamic_object (ObjectHandle);

         if (root.valid ()) { root->removeChild (os->geode.get ()); }
      }

      delete os; os = 0;
   }
}


void
dmz::RenderPluginObjectTextOSG::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

}


void
dmz::RenderPluginObjectTextOSG::update_object_text (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const String &Value,
      const String *PreviousValue) {

   ObjectStruct *os = _objTable.lookup (ObjectHandle);

   if (os) {

      TextStruct *ts = os->table.lookup (AttributeHandle);

      if (!ts) { _create_text_struct (AttributeHandle, *os); }
      else {

         ts->data = Value;

         _update_text (*os, *ts);
      }
   }
}


// RenderPluginObjectTextOSG Interface
void
dmz::RenderPluginObjectTextOSG::_update_text (ObjectStruct &os, TextStruct &ts) {
   
   if (!os.geode.valid ()) {

      os.geode = new osg::Geode;

      osg::ref_ptr<osg::StateSet> set = os.geode->getOrCreateStateSet ();

      set->setMode (GL_LIGHTING, osg::StateAttribute::OFF);

      if (_core) {

         osg::ref_ptr<osg::Group> root = _core->create_dynamic_object (os.Object);

         if (root.valid ()) { root->addChild (os.geode.get ()); }
      }
   }

   if (os.geode.valid ()) {

      if (!ts.text.valid ()) {

         ts.text = new osgText::Text;

         if (ts.text.valid ()) {

            ts.text->setColor (ts.color);
            if (ts.Def.FontName) { ts.text->setFont (ts.Def.FontName.get_buffer ()); }
            ts.text->setAxisAlignment (osgText::TextBase::SCREEN);
            ts.text->setPosition (to_osg_vector (ts.Def.Offset));
            ts.text->setBackdropType (osgText::Text::NONE);
            ts.text->setCharacterSize (ts.Def.Height);
            ts.text->setAlignment (osgText::TextBase::CENTER_BOTTOM);

            os.geode->addDrawable (ts.text.get ());
         }
      }

      if (ts.text.valid () && ts.data) {

         ts.text->setText (ts.data.get_buffer ());
      }
   }
}


void
dmz::RenderPluginObjectTextOSG::_create_text_struct (
      const Handle Attribute,
      ObjectStruct &os) {

   TextDefStruct *tds = os.Defs.table.lookup (Attribute);

   if (tds) {

      TextStruct *ts = new TextStruct (*tds);

      ObjectModule *module (get_object_module ());

      if (module && ts && os.table.store (tds->Attribute, ts)) {

         Vector rgb (tds->Color.r (), tds->Color.g (), tds->Color.b ());
         Float64 alpha (tds->Color.a ());
         String text;

         module->lookup_vector (os.Object, tds->Attribute, rgb);
         module->lookup_scalar (os.Object, tds->Attribute, alpha);

         ts->color.set (
            rgb.get_x (),
            rgb.get_y (),
            rgb.get_z (),
            alpha);

         if (module->lookup_text (os.Object, tds->Attribute, ts->data)) {

            _update_text (os, *ts);
         }
      }
      else if (ts) { delete ts; ts = 0; }
   }
}


dmz::RenderPluginObjectTextOSG::TypeStruct *
dmz::RenderPluginObjectTextOSG::_lookup_type_struct (const ObjectType &Type) {

   TypeStruct *result (_typeMap.lookup (Type.get_handle ()));

   if (!result) {

      ObjectType current (Type);

      while (current && !result) {

         result = _typeTable.lookup (current.get_handle ());

         if (!result) { result = _create_type_struct (current); }

         if (!result) { current.become_parent (); }
      }

      if (result) { _typeMap.store (Type.get_handle (), result); }
   }

   return result;
}


dmz::RenderPluginObjectTextOSG::TypeStruct *
dmz::RenderPluginObjectTextOSG::_create_type_struct (const ObjectType &Type) {

   TypeStruct *result (0);

   Config list;

   if (Type.get_config ().lookup_all_config ("render.text", list)) {

      result = new TypeStruct;

      if (result && _typeTable.store (Type.get_handle (), result)) {

         ConfigIterator it;
         Config def;

         while (list.get_next_config (it, def)) {

            const String AttributeName = config_to_string ("attribute", def);

            const Handle Attribute = activate_object_attribute (
               AttributeName,
               ObjectTextMask | ObjectVectorMask | ObjectScalarMask);

            const osg::Vec4 Color = config_to_osg_vec4_color (
               "color",
               def,
               osg::Vec4 (1.0, 1.0, 1.0, 1.0));

            const Vector Offset = config_to_vector ("offset", def);

            const String FontResource = config_to_string ("font", def);

            const String FontFile =
               FontResource ? _rc.find_file (FontResource) : String ();

            if (FontResource && !FontFile) {

               _log.error << "Unable to find font resource: " << FontResource << endl;
            }

            const Float64 Height = config_to_float64 ("height", def, 1.0);

            TextDefStruct *tds = new TextDefStruct (
               Attribute,
               Color, 
               Offset,
               FontFile,
               Height);

            if (tds && !result->table.store (Attribute, tds)) {

               delete tds; tds = 0;
            }
         }
      }
      else if (result) { delete result; result = 0; }
   }

   return result;
}


void
dmz::RenderPluginObjectTextOSG::_init (Config &local) {

   activate_default_object_attribute (ObjectCreateMask | ObjectDestroyMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginObjectTextOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginObjectTextOSG (Info, local);
}

};
