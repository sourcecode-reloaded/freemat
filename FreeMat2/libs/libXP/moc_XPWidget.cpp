/****************************************************************************
** XPWidget meta object code from reading C++ file 'XPWidget.hpp'
**
** Created: Tue May 24 14:06:02 2005
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "XPWidget.hpp"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *XPWidget::className() const
{
    return "XPWidget";
}

QMetaObject *XPWidget::metaObj = 0;
static QMetaObjectCleanUp cleanUp_XPWidget( "XPWidget", &XPWidget::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString XPWidget::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "XPWidget", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString XPWidget::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "XPWidget", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* XPWidget::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"XPWidget", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_XPWidget.setMetaObject( metaObj );
    return metaObj;
}

void* XPWidget::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "XPWidget" ) )
	return this;
    return QWidget::qt_cast( clname );
}

bool XPWidget::qt_invoke( int _id, QUObject* _o )
{
    return QWidget::qt_invoke(_id,_o);
}

bool XPWidget::qt_emit( int _id, QUObject* _o )
{
    return QWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool XPWidget::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool XPWidget::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
