#include <osg/Node>
#include <osg/NodeVisitor>
#include <osgDB/ReadFile>

#include <qdb.h>

using namespace dmz;

static qdb out;


class osgDump : public osg::NodeVisitor {

   public:
      int offset;
      osgDump () : offset (0), osg::NodeVisitor (osg::NodeVisitor::NODE_VISITOR, osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {;}
      ~osgDump () {;}

      virtual void apply (osg::Node &node);
};


void
osgDump::apply (osg::Node &node) {

   String buffer;
   buffer.repeat (" ", offset);
   out << buffer << node.className ();
   String name = node.getName ().c_str ();
   if (name) { out << "[" << name << "]"; }
   out << endl;

   offset += 3;
   traverse (node);
   offset -= 3;
}

int
main (int argc, char *argv[]) {

   if (argc > 1) {

      osg::ref_ptr<osg::Node> node = osgDB::readNodeFile (argv[1]);

      if (node.valid ()) {

         osgDump dump;
         node->accept (dump);
      }
      else { out << "Failed to load: " << argv[1] << endl; }
   }
}
