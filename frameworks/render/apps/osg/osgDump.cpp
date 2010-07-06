#include <dmzTypesString.h>
#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/NodeVisitor>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
// #include <osgFX/Cartoon>

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
   osg::Transform *tf = node.asTransform ();

   if (tf) {

      osg::MatrixTransform *mtf = tf->asMatrixTransform ();

      if (mtf) {

         osg::Matrix mat = mtf->getMatrix ();

         if (mat.isIdentity ()) { out << " Identity"; }
#if 0
         else {

            for (int row = 0; row < 4; row++) {
               for (int col = 0; col < 4; col++) {

                  out << " " << mat (row, col);
               }
            }
         }
#endif
      }
   }

   out << endl;

osg::Node::DescriptionList& list = node.getDescriptions ();
osg::Node::DescriptionList::iterator it;
   for (it = list.begin (); it != list.end (); it++) {

out << "Description: " << it->c_str () << endl;
   }



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

#if 0
osgFX::Cartoon *r = new osgFX::Cartoon;
r->setOutlineColor (osg::Vec4 (1.0, 0.0, 0.0, 0.0));
r->addChild (node.get ());
osgDB::writeNodeFile (*r, "recognizer.ive");
#endif
      }
      else { out << "Failed to load: " << argv[1] << endl; }
   }
}
