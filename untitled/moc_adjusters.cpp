/****************************************************************************
** Meta object code from reading C++ file 'adjusters.h'
**
** Created: Thu Dec 4 19:59:24 2014
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "adjusters.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'adjusters.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Adjusters[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x08,
      22,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Adjusters[] = {
    "Adjusters\0\0handleUp()\0handleDown()\0"
};

const QMetaObject Adjusters::staticMetaObject = {
    { &QVBoxLayout::staticMetaObject, qt_meta_stringdata_Adjusters,
      qt_meta_data_Adjusters, 0 }
};

const QMetaObject *Adjusters::metaObject() const
{
    return &staticMetaObject;
}

void *Adjusters::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Adjusters))
        return static_cast<void*>(const_cast< Adjusters*>(this));
    return QVBoxLayout::qt_metacast(_clname);
}

int Adjusters::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QVBoxLayout::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: handleUp(); break;
        case 1: handleDown(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
