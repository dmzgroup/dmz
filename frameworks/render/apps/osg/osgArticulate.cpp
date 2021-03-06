#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesString.h>

#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/NodeVisitor>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgSim/DOFTransform>

#include <qdb.h>

using namespace dmz;

static qdb out;


class osgArticulate : public osg::NodeVisitor {

   public:
      HashTableStringTemplate<osg::Group> table;
      osgArticulate () : osg::NodeVisitor (
            osg::NodeVisitor::NODE_VISITOR,
            osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {;}

      ~osgArticulate () {;}

      virtual void apply (osg::Node &node);
};


void
osgArticulate::apply (osg::Node &node) {

   String name = node.getName ().c_str ();

   if (name) {

      if (name.contains_sub ("wheel_")) {

         osg::Group *group = node.asGroup ();

         if (group) {

            if (!table.store (name, group)) {

               out << "Failed to store group: " << name << endl;
            }
         }
      }
   }

   traverse (node);
}

int
main (int argc, char *argv[]) {

   if (argc > 2) {

      osg::ref_ptr<osg::Node> node = osgDB::readNodeFile (argv[1]);

      if (node.valid ()) {

         osgArticulate dump;
         node->accept (dump);

         HashTableStringIterator it;
         osg::Group *ptr (0);

         while (dump.table.get_next (it, ptr)) {

            out << it.get_hash_key () << endl;

#if 0
            osg::ref_ptr<osgSim::DOFTransform> dof = new osgSim::DOFTransform;
            dof->setCurrentScale (osg::Vec3 (1.0, 1.0, 1.0));
#else
            osg::ref_ptr<osg::MatrixTransform> dof = new osg::MatrixTransform;
#endif
            dof->setReferenceFrame (osg::Transform::RELATIVE_RF);

            String name ("dmz_dof_");
            name << it.get_hash_key ();

            dof->setName (name.get_buffer ());

            const unsigned int ChildCount = ptr->getNumChildren ();

            for (unsigned int ix = 0; ix < ChildCount; ix++) {

               dof->addChild (ptr->getChild (ix));
            }

            ptr->removeChildren (0, ChildCount);

            ptr->addChild (dof.get ());
         }

         osgDB::writeNodeFile (*node, argv[2]);
      }
      else { out << "Failed to load: " << argv[1] << endl; }
   }
   else { out << "Requires two arguments." << endl; }
}
