/****************************************************************************
** Meta object code from reading C++ file 'dialogdisplaytree.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "dialogdisplaytree.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dialogdisplaytree.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DialogDisplayTree_t {
    QByteArrayData data[9];
    char stringdata[171];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_DialogDisplayTree_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_DialogDisplayTree_t qt_meta_stringdata_DialogDisplayTree = {
    {
QT_MOC_LITERAL(0, 0, 17),
QT_MOC_LITERAL(1, 18, 24),
QT_MOC_LITERAL(2, 43, 0),
QT_MOC_LITERAL(3, 44, 19),
QT_MOC_LITERAL(4, 64, 5),
QT_MOC_LITERAL(5, 70, 31),
QT_MOC_LITERAL(6, 102, 24),
QT_MOC_LITERAL(7, 127, 27),
QT_MOC_LITERAL(8, 155, 14)
    },
    "DialogDisplayTree\0on_remplirButton_clicked\0"
    "\0on_treeView_clicked\0index\0"
    "on_comboBox_currentIndexChanged\0"
    "on_addTypeButton_clicked\0"
    "on_deleteTreeButton_clicked\0onTypesChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DialogDisplayTree[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x08,
       3,    1,   45,    2, 0x08,
       5,    1,   48,    2, 0x08,
       6,    0,   51,    2, 0x08,
       7,    0,   52,    2, 0x08,
       8,    0,   53,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DialogDisplayTree::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DialogDisplayTree *_t = static_cast<DialogDisplayTree *>(_o);
        switch (_id) {
        case 0: _t->on_remplirButton_clicked(); break;
        case 1: _t->on_treeView_clicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: _t->on_comboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_addTypeButton_clicked(); break;
        case 4: _t->on_deleteTreeButton_clicked(); break;
        case 5: _t->onTypesChanged(); break;
        default: ;
        }
    }
}

const QMetaObject DialogDisplayTree::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DialogDisplayTree.data,
      qt_meta_data_DialogDisplayTree,  qt_static_metacall, 0, 0}
};


const QMetaObject *DialogDisplayTree::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DialogDisplayTree::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DialogDisplayTree.stringdata))
        return static_cast<void*>(const_cast< DialogDisplayTree*>(this));
    return QWidget::qt_metacast(_clname);
}

int DialogDisplayTree::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
