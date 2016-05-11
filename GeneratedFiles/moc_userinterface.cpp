/****************************************************************************
** Meta object code from reading C++ file 'userinterface.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../szakdolgozat/UI/userinterface.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'userinterface.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_UserInterface_t {
    QByteArrayData data[19];
    char stringdata0[242];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UserInterface_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UserInterface_t qt_meta_stringdata_UserInterface = {
    {
QT_MOC_LITERAL(0, 0, 13), // "UserInterface"
QT_MOC_LITERAL(1, 14, 13), // "prevAtomEvent"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 13), // "nextAtomEvent"
QT_MOC_LITERAL(4, 43, 14), // "nextPlaneEvent"
QT_MOC_LITERAL(5, 58, 14), // "newprior_event"
QT_MOC_LITERAL(6, 73, 16), // "newcutmode_event"
QT_MOC_LITERAL(7, 90, 14), // "newplane_event"
QT_MOC_LITERAL(8, 105, 12), // "cuttingEvent"
QT_MOC_LITERAL(9, 118, 9), // "undoEvent"
QT_MOC_LITERAL(10, 128, 11), // "acceptEvent"
QT_MOC_LITERAL(11, 140, 18), // "typeaccept_handler"
QT_MOC_LITERAL(12, 159, 13), // "nextNCutEvent"
QT_MOC_LITERAL(13, 173, 12), // "restartEvent"
QT_MOC_LITERAL(14, 186, 9), // "infoEvent"
QT_MOC_LITERAL(15, 196, 10), // "backToMenu"
QT_MOC_LITERAL(16, 207, 10), // "newdisplay"
QT_MOC_LITERAL(17, 218, 11), // "exportEvent"
QT_MOC_LITERAL(18, 230, 11) // "importEvent"

    },
    "UserInterface\0prevAtomEvent\0\0nextAtomEvent\0"
    "nextPlaneEvent\0newprior_event\0"
    "newcutmode_event\0newplane_event\0"
    "cuttingEvent\0undoEvent\0acceptEvent\0"
    "typeaccept_handler\0nextNCutEvent\0"
    "restartEvent\0infoEvent\0backToMenu\0"
    "newdisplay\0exportEvent\0importEvent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UserInterface[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   99,    2, 0x08 /* Private */,
       3,    0,  100,    2, 0x08 /* Private */,
       4,    0,  101,    2, 0x08 /* Private */,
       5,    0,  102,    2, 0x08 /* Private */,
       6,    0,  103,    2, 0x08 /* Private */,
       7,    0,  104,    2, 0x08 /* Private */,
       8,    0,  105,    2, 0x08 /* Private */,
       9,    0,  106,    2, 0x08 /* Private */,
      10,    0,  107,    2, 0x08 /* Private */,
      11,    0,  108,    2, 0x08 /* Private */,
      12,    0,  109,    2, 0x08 /* Private */,
      13,    0,  110,    2, 0x08 /* Private */,
      14,    0,  111,    2, 0x08 /* Private */,
      15,    0,  112,    2, 0x08 /* Private */,
      16,    0,  113,    2, 0x08 /* Private */,
      17,    0,  114,    2, 0x08 /* Private */,
      18,    0,  115,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void UserInterface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        UserInterface *_t = static_cast<UserInterface *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->prevAtomEvent(); break;
        case 1: _t->nextAtomEvent(); break;
        case 2: _t->nextPlaneEvent(); break;
        case 3: _t->newprior_event(); break;
        case 4: _t->newcutmode_event(); break;
        case 5: _t->newplane_event(); break;
        case 6: _t->cuttingEvent(); break;
        case 7: _t->undoEvent(); break;
        case 8: _t->acceptEvent(); break;
        case 9: _t->typeaccept_handler(); break;
        case 10: _t->nextNCutEvent(); break;
        case 11: _t->restartEvent(); break;
        case 12: _t->infoEvent(); break;
        case 13: _t->backToMenu(); break;
        case 14: _t->newdisplay(); break;
        case 15: _t->exportEvent(); break;
        case 16: _t->importEvent(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject UserInterface::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_UserInterface.data,
      qt_meta_data_UserInterface,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *UserInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UserInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_UserInterface.stringdata0))
        return static_cast<void*>(const_cast< UserInterface*>(this));
    return QWidget::qt_metacast(_clname);
}

int UserInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
