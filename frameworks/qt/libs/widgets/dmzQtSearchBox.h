/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef DMZ_QT_SEARCH_BOX_H
#define DMZ_QT_SEARCH_BOX_H

#ifdef _WIN32

#ifdef DMZ_QT_SEARCH_BOX_EXPORT
#define DMZ_QT_SEARCH_BOX_LINK_SYMBOL __declspec (dllexport)
#else
#define DMZ_QT_SEARCH_BOX_LINK_SYMBOL __declspec (dllimport)
#endif // DMZ_QT_SEARCH_BOX_EXPORT

#else // !_WIN32

#define DMZ_QT_SEARCH_BOX_LINK_SYMBOL

#endif // _WIN32

#include <QtGui/QLineEdit>

class GSuggestCompletion;

namespace dmz {

class DMZ_QT_SEARCH_BOX_LINK_SYMBOL QtSearchBox: public QLineEdit {

   Q_OBJECT

   public:
      QtSearchBox (QWidget *parent = 0);

   protected Q_SLOTS:
      void _do_search ();

   protected:
      GSuggestCompletion *_completer;
};

};

#endif // DMZ_QT_SEARCH_BOX_H
