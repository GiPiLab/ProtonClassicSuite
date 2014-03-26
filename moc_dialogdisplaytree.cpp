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
    QByteArrayData data[13];
    char stringdata[286];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_DialogDisplayTree_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_DialogDisplayTree_t qt_meta_stringdata_DialogDisplayTree = {
    {
QT_MOC_LITERAL(0, 0, 17),
QT_MOC_LITERAL(1, 18, 31),
QT_MOC_LITERAL(2, 50, 0),
QT_MOC_LITERAL(3, 51, 5),
QT_MOC_LITERAL(4, 57, 24),
QT_MOC_LITERAL(5, 82, 27),
QT_MOC_LITERAL(6, 110, 14),
QT_MOC_LITERAL(7, 125, 24),
QT_MOC_LITERAL(8, 150, 27),
QT_MOC_LITERAL(9, 178, 24),
QT_MOC_LITERAL(10, 203, 27),
QT_MOC_LITERAL(11, 231, 25),
QT_MOC_LITERAL(12, 257, 27)
    },
    "DialogDisplayTree\0on_comboBox_currentIndexChanged\0"
    "\0index\0on_addTypeButton_clicked\0"
    "on_deleteTreeButton_clicked\0onTypesChanged\0"
    "on_newTreeButton_clicked\0"
    "on_deleteTypeButton_clicked\0"
    "on_addNodeButton_clicked\0"
    "on_modifyNodeButton_clicked\0"
    "on_treeView_doubleClicked\0"
    "on_deleteNodeButton_clicked\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DialogDisplayTree[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x08,
       4,    0,   67,    2, 0x08,
       5,    0,   68,    2, 0x08,
       6,    0,   69,    2, 0x08,
       7,    0,   70,    2, 0x08,
       8,    0,   71,    2, 0x08,
       9,    0,   72,    2, 0x08,
      10,    0,   73,    2, 0x08,
      11,    1,   74,    2, 0x08,
      12,    0,   77,    2, 0x08,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex,    3,
    QMetaType::Void,

       0        // eod
};

void DialogDisplayTree::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DialogDisplayTree *_t = static_cast<DialogDisplayTree *>(_o);
        switch (_id) {
        case 0: _t->on_comboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->on_addTypeButton_clicked(); break;
        case 2: _t->on_deleteTreeButton_clicked(); break;
        case 3: _t->onTypesChanged(); break;
        case 4: _t->on_newTreeButton_clicked(); break;
        case 5: _t->on_deleteTypeButton_clicked(); break;
        case 6: _t->on_addNodeButton_clicked(); break;
        case 7: _t->on_modifyNodeButton_clicked(); break;
        case 8: _t->on_treeView_doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 9: _t->on_deleteNodeButton_clicked(); break;
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
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
