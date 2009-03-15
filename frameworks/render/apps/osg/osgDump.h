#ifndef OSG_DUMP_DOT_H
#define OSG_DUMP_DOT_H

#include <osg/NodeVisitor>

class osgDump : public osg::NodeVisitor {

   public:
      osgDump () : osg::NodeVisitor (osg::NodeVisitor::NODE_VISITOR, osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {;}
      ~osgDump () {;}

      virtual void apply (osg::Node &node);
};

#endif // OSG_DUMP_DOT_H
